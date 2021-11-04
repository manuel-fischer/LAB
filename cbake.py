import os
import sys
import struct

# TODO relative inclusion of header files in include/ from src/ not supported by vscode

try:
    import commentjson as json
except:
    import json
from dataclasses import dataclass

CBAKE_DEP_FILE = ".cbake-dependencies.txt"
CBAKE_DEP_FILE_DBG = ".cbake-dependencies-dbg.txt"

cbake_dep_file = CBAKE_DEP_FILE

def pjoin(*paths):

    path = []

    for p in paths:
        if os.sep != "/": p = p.replace("/", os.sep)
        for d in p.split("/"):
            if not d:
                pass
            elif d == ".":
                pass
            elif d == "..":
                try:
                    path.pop()
                except IndexError:
                    raise FileNotFoundError
            else:
                path.append(d)

    return os.sep.join(path)

def eprint(*args, end='\n', file=sys.stderr):
    print(*args, end=end, file=file)

CC = "gcc"
CXX = "g++"


FILE_NOT_FOUND = object()
FILE_AMBIGUOUS = object()

# used by the special syntax:
#  `@!WIN&64: -opt`, this enables `-opt` only if the current platform
#  is not Windows and the current platform is a 64 bit system
flags = {
    "WIN": os.name == 'nt',
    str(struct.calcsize("P") * 8): True # Pointer bit width: 32 for 32-bit; 64 for 64-bit eg x64
}


# prefix to all output files
out_prefix = ""


# filename -> effective_path | FILE_NOT_FOUND | FILE_AMBIGUOUS
# None if the file is cannot be found
path_cache = {}

# Files could be moved between src/ and include/!
# this needs to be handled correctly
#
# files with the same name in the src/ and include/
# directories are not allowed
def get_effective_path_(path):
    global has_invalid_includes
    
    src_path = pjoin("src", path)
    inc_path = pjoin("include", path)
    
    in_src     = os.path.exists(src_path)
    in_include = os.path.exists(inc_path)

    if in_src and in_include and src_path != inc_path:
        return FILE_AMBIGUOUS

    if in_src:     return src_path
    if in_include: return inc_path

    return FILE_NOT_FOUND

def get_effective_path_s(path):
    try:    return path_cache[path]
    except: pass
    
    epath = get_effective_path_(path)
    path_cache[path] = epath
    return epath
    
def get_effective_path(path):
    epath = get_effective_path_s(path)
    if epath == FILE_NOT_FOUND: raise FileNotFoundError
    if epath == FILE_AMBIGUOUS: raise Exception("Ambiguous Filename")
    return epath

def dbg(locals_dict):
    for k, v in locals_dict.items():
        eprint(f"{k+':':20} {v}")


def conditional_element(s: str) -> str:
    if s.startswith('@'):
        s = s[1:]

        flag_end = s.find(':')
        flag_list = s[:flag_end].split('&')

        for flag in flag_list:
            flag = flag.strip()
            negated = flag.startswith('!')
            if negated: flag = flag[1:].strip()

            result = flag in flags and flags[flag]
            if negated: result = not result
            if not result:
                return ""
        
        return s[flag_end+1:]
    else:
        return s
    

def str_list(str_or_list) -> str:
    if type(str_or_list) == str: return str_or_list
    else: return " ".join(map(conditional_element, str_or_list))


def read_dep_file():
    file_times = {}
    file_includes = {}
    try:
        with open(cbake_dep_file) as f:
            for l in f.readlines():
                l = l.strip()

                if not l: continue
                
                fn, time, *includes = map(str.strip, l.split())
                time = float(time)
                file_times[fn] = time
                def parse_include(inc):
                    at_pos = inc.rfind('@')
                    ln = int(inc[at_pos+1:])
                    return inc[:at_pos], ln
                file_includes[fn] = list(map(parse_include, includes))
                                  # [parse_include(inc) for inc in includes]
    except FileNotFoundError: pass

    return file_times, file_includes


def write_dep_file(file_times, file_includes):
    
    files = sorted(file_times.keys())
    with open(cbake_dep_file, "w") as f:
        for fn in files:
            s_time = str(file_times[fn])
            s_includes = " ".join(f"{fname}@{ln}" for fname, ln in file_includes[fn])
            print(f"{fn} {s_time} {s_includes}", file = f)
    

def get_err_msg(efn):
    if efn == FILE_NOT_FOUND:
        return "No such file or directory"
    if efn == FILE_AMBIGUOUS:
        return "Ambiguous file include"
            

def get_includes(filename, efilename):
    with open(efilename) as f:
        for ln, rl in enumerate(f.readlines()):
            l = rl.strip()

            if not l: continue
            if l[0] != '#': continue

            l = l[1:].strip()

            if not l.startswith("include"): continue

            l = l[len("include"):].strip()

            if not l[0] == '"': continue

            l = l[1:]
            end = l.find('"')
            fname = l[:end]
            rfname = fname

            if os.sep != "/":
                fname = fname.replace("/", os.sep)

            if fname.startswith("."):
                fname = os.path.split(filename)[0] + "/" + fname
            
            #fnd = rl.find(rfname)
            #if msg := get_err_msg(efn):
            #    herefile = efilename # pjoin(os.getcwd(), efilename)
            #    eprint(
            #        f"{herefile}:{ln+1}:{fnd+1-1}: fatal error: {rfname}: {msg}\n" +
            #        f" {rl.rstrip()}\n" +
            #        " "*(fnd+1-1) + "^" + "~"*(2-1+len(rfname)),
            #        end = "\n\n"
            #    )
                
            yield fname, ln+1


def check_includes(filename, efilename, includes):
    contents = None
    success = True
    for fname, ln in includes:
        efn = get_effective_path_s(fname)
        if msg := get_err_msg(efn):
            if contents is None:
                with open(efilename) as f:
                    contents = f.readlines()

            success = False

            l = contents[ln-1]
            a = l.find('"')
            b = l.find('"', a+1)
            rfname = l[a+1:b]
            
            herefile = efilename # pjoin(os.getcwd(), efilename)
            eprint(
                f"{herefile}:{ln}:{a+1-1}: fatal error: {rfname}: {msg}\n" +
                f" {l.rstrip()}\n" +
                " "*(a+1-1) + "^" + "~"*(2-1+len(rfname)),
                end = "\n\n"
            )

    return success
            

def get_included_files(includes):
    return set(fname for fname, location in includes)

def collect_files(path):
    lst = os.listdir(path)
    for fn in lst:
        p = pjoin(path, fn)
        if os.path.isdir(p):
            yield from collect_files(p)
        else:
            yield p

def collect_sources():
    for f in collect_files("src"):
        if os.path.splitext(f)[1] in [".c", ".cpp"]:
            assert f[0:4] == "src" + os.sep
            yield f[4:]

def discover(file_times, file_includes, sources):
    success = True


    # rebuilding: automatically removing unreferenced files
    new_file_times = {}
    new_file_includes = {}
        

    
    known_files = set(file_times.keys())
    
    src_files = set(sources)
    cur_files = set(src_files) # copy
    checked_files = set()

    modified_files = set()

    # forward pass: find included files
    while cur_files:
        next_files = set()
        checked_files |= cur_files

        for fn in cur_files:
            efn = get_effective_path_s(fn)
            assert get_err_msg(efn) == None

            f_time = os.path.getmtime(efn)
            
            if fn not in known_files or \
               f_time > file_times[fn]:

                includes = list(get_includes(fn, efn))
                modified_files |= {fn}
            else:
                includes = file_includes[fn]

            if not check_includes(fn, efn, includes):
                success = False

            else:

                included_files = get_included_files(includes)
                next_files |= included_files - checked_files
                new_file_includes[fn] = includes

                new_file_times[fn] = f_time
                known_files |= {fn}


        cur_files = next_files


    # middle pass: create backpointers, invert graph
    included_from = {}
    for fn, includes in new_file_includes.items():
        for ff in includes:
            if ff in included_from:
                included_from[ff] |= {fn}
            else:
                included_from[ff] = {fn}            


    # backward pass: propagate modifications
    recompile = set()

    cur_files = modified_files
    propagated_files = set()
    
    while cur_files:
        next_files = set()
        propagated_files |= cur_files
        
        for fn in cur_files:
            if fn in src_files:
                recompile |= {fn}

            if fn in included_from: # there are files including this file
                next_files |= included_from[fn] - propagated_files

        cur_files = next_files


    #dbg(locals())


    return new_file_times, new_file_includes, recompile, success


def compile_object_file(fn, settings):
    fnn, ext = os.path.splitext(fn)
    if ext == ".c":
        flags = str_list(settings.get("c-flags", ""))
        comp  = CC
    else:
        flags = str_list(settings.get("cxx-flags", ""))
        comp  = CXX
        
    ofn = f"obj/{out_prefix + fnn}.o"
    
    os.makedirs(os.path.split(ofn)[0], exist_ok=True)

    # add include path relative to the include directory with the same name
    include_path_rel = "-I" + pjoin("include", os.path.split(fn)[0])
    include_path     = "-Iinclude"
    include_paths = include_path if include_path_rel == include_path else include_path_rel + " " + include_path
    
    cmd = f"{comp} -c -o {ofn} {include_paths} src/{fn} {flags}"

    eprint(cmd)
    success = os.system(cmd) == 0
    return success

def compile_executable(sources, settings):

    has_cxx = False
    object_files = []
    for fn in sources:
        fnn, ext = os.path.splitext(fn)
        if ext == ".cpp": has_cxx = True
        object_files.append(f"obj/{out_prefix + fnn}.o")
        
    flags = str_list(settings.get("linker-flags", ""))

    if has_cxx: comp = CXX
    else:       comp = CC

    ofn = settings.get("program", "a.out")
    cmd = f"{comp} -o {out_prefix + ofn} {' '.join(object_files)} {flags}"

    eprint(cmd)
    success = os.system(cmd) == 0
    return success


def load_settings():
    with open("bakefile.json") as f:
        return json.loads(f.read())


def program_filename(name):
    n, ext = os.path.splitext(name)
    if ext == ".exe": return name
    if flags["WIN"]: # Windows
        return name + ".exe"
    return name


def process_files(settings):
    # 1. discover
    # 2. compile
    # 3. update dependency file
    
    # 1. discover
    eprint("CBake: File discovery...")
    sources = list(collect_sources())
    file_times, file_includes = read_dep_file()
    n_file_times, n_file_includes, recompile, success = \
                  discover(file_times, file_includes, sources)

    if not success:
        eprint("CBake: File discovery failed")
        return False
    
    # 2. compile
    eprint("CBake: Object file compilation...")
    for fn in recompile:
        if not compile_object_file(fn, settings):
            success = False
            # remove it from the list to invalidate
            del n_file_times[fn]
            del n_file_includes[fn]


    if success and recompile:
        eprint("CBake: Executable linking...")
        success = compile_executable(sources, settings)
    elif success:
            eprint("CBake: Nothing needs to be done")
            
    if not success:
        eprint("CBake: Compilation failed")

    # 3. update dependency
    if n_file_times != file_times or n_file_includes != file_includes:
        write_dep_file(n_file_times, n_file_includes)

    return success


def cbake(root):
    prev_cwd = os.getcwd()
    os.chdir(root)
    try:
        process_files()
    finally:
        os.chdir(prev_cwd)


def remove(filename):
        try: os.remove(filename)
        except FileNotFoundError: pass


if __name__ == "__main__":
    from sys import argv
    settings = load_settings()

    if "help" in argv:
        print("cbake.py help | [debug] [test] | clear")
        print()
        print("   help     Shows this help")
        print("   debug    Enables the debugging target")
        print("            The program filename is prefixed with 'dbg-'")
        print("            It sets the DEBUG flag to true")
        print("   test     Run the program after compilation")
        print("   clear    Delete the executable and the dependency cache")
        exit(2)
    

    if "debug" in argv:
        flags["DEBUG"] = True
        argv.remove("debug")

        out_prefix = "dbg-"
        cbake_dep_file = CBAKE_DEP_FILE_DBG

    program = settings.get("program", "a.out")
    
    if len(argv) == 1:
        success = process_files(settings)
        exit(0 if success else 1)
    elif argv[1] == "clear":
        remove(program_filename(program))
        remove(program_filename("dbg-" + program))
        # TODO delete object files
        remove(CBAKE_DEP_FILE)
        remove(CBAKE_DEP_FILE_DBG)
    elif argv[1] == "test":
        success = process_files(settings)
        if success: success = os.system(out_prefix + program) == 0
        exit(0 if success else 1)
        
        
    

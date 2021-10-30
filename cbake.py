import os
try:
    import commentjson as json
except:
    import json
from dataclasses import dataclass

CBAKE_DEP_FILE = ".cbake-dependencies.txt"

pjoin = os.path.join


CC = "gcc"
CXX = "g++"

# Files could be moved between src/ and include/!
# this needs to be handled correctly
#
# files with the same name in the src/ and include/
# directories are not allowed
def get_effective_path(path):
    src_path = pjoin("src", path)
    dst_path = pjoin("include", path)
    
    in_src     = os.path.exists(src_path)
    in_include = os.path.exists(dst_path)

    if in_src and in_include:
        raise Exception(f"Used files with the same name in src/ and include/: {path}")

    if in_src:     return src_path
    if in_include: return dst_path

    raise FileNotFoundError
    

def dbg(locals_dict):
    for k, v in locals_dict.items():
        print(f"{k+':':20} {v}")


def str_list(str_or_list) -> str:
    if type(str_or_list) == str: return str_or_list
    else: return " ".join(str_or_list)


def read_dep_file():
    file_times = {}
    file_includes = {}
    try:
        with open(CBAKE_DEP_FILE) as f:
            for l in f.readlines():
                l = l.strip()

                if not l: continue
                
                fn, time, *includes = map(str.strip, l.split())
                time = float(time)
                file_times[fn] = time
                file_includes[fn] = set(includes)
    except FileNotFoundError: pass

    return file_times, file_includes


def write_dep_file(file_times, file_includes):
    
    files = sorted(file_times.keys())
    with open(CBAKE_DEP_FILE, "w") as f:
        for fn in files:
            s_time = str(file_times[fn])
            s_includes = " ".join(file_includes[fn])
            print(f"{fn} {s_time} {s_includes}", file = f)
    

            

def get_includes(filename):
    with open(filename) as f:
        for l in f.readlines():
            l = l.strip()

            if not l: continue
            if l[0] != '#': continue

            l = l[1:].strip()

            if not l.startswith("include"): continue

            l = l[len("include"):].strip()

            if not l[0] == '"': continue

            l = l[1:]
            end = l.find('"')
            fname = l[:end]

            if os.sep != "/":
                fname = fname.replace("/", os.sep)
            yield fname

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
            try:
                efn = get_effective_path(fn)
            except FileNotFoundError:
                print(f"Fatal Error: File not found: {fn}")
                # TODO give more detailed information
                raise
            except:
                pass # TODO duplicate
                raise

            f_time = os.path.getmtime(efn)
            
            if fn not in known_files or \
               f_time > file_times[fn]:

                includes = set(get_includes(efn))
                modified_files |= {fn}
            else:
                includes = file_includes[fn]

            next_files |= includes - checked_files
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


    return new_file_times, new_file_includes, recompile


def compile_object_file(fn, settings):
    fnn, ext = os.path.splitext(fn)
    if ext == ".c":
        flags = str_list(settings.get("c-flags", ""))
        comp  = CC
    else:
        flags = str_list(settings.get("cxx-flags", ""))
        comp  = CXX
        
    ofn = f"obj/{fnn}.o"
    
    os.makedirs(os.path.split(ofn)[0], exist_ok=True)
    
    cmd = f"{comp} -c -o {ofn} -Iinclude/ src/{fn} {flags}"

    print(cmd)
    # TODO if compilation fails, remove it from the file_times, to invalidate
    if os.system(cmd): exit(1)

def compile_executable(sources, settings):

    has_cxx = False
    object_files = []
    for fn in sources:
        fnn, ext = os.path.splitext(fn)
        if ext == ".cpp": has_cxx = True
        object_files.append(f"obj/{fnn}.o")
        
    flags = str_list(settings.get("linker-flags", ""))

    if has_cxx: comp = CXX
    else:       comp = CC

    ofn = settings.get("program", "a.out")
    cmd = f"{comp} -o {ofn} {' '.join(object_files)} {flags}"

    print(cmd)
    if os.system(cmd): exit(1)


def load_settings():
    with open("bakefile.json") as f:
        return json.loads(f.read())


def program_filename(name):
    n, ext = os.path.splitext(name)
    if ext == ".exe": return name
    if os.name == 'nt': # Windows
        return name + ".exe"
    return name


def process_files(settings):
    # 1. discover
    # 2. compile
    # 3. update dependency file
    
    # 1. discover
    sources = list(collect_sources())
    file_times, file_includes = read_dep_file()
    n_file_times, n_file_includes, recompile = discover(file_times, file_includes, sources)
    
    # 2. compile    
    for fn in recompile:
        compile_object_file(fn, settings)


    if recompile:
        compile_executable(sources, settings)
    else:
        print("CBake: Nothing needs to be done")

    # 3. update dependency
    if n_file_times != file_times or n_file_includes != file_includes:
        write_dep_file(n_file_times, n_file_includes)



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
    if len(argv) == 1:
        process_files(settings)
    elif argv[1] == "clear":
        remove(program_filename(settings["program"]))
        # TODO delete object files
        remove(CBAKE_DEP_FILE)
    elif argv[1] == "test":
        process_files(settings)
        os.system(program_filename(settings["program"]))
        
    

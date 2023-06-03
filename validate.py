import os
from pathlib import Path
from fnmatch import fnmatch

def match_entity(t, pat):
    return fnmatch(t, pat)

MISSING_IMPORTS = True
UNUSED_IMPORTS = True

FILTER_NAMESPACES = ["LAB", "HTL"]

# TODO: detect includes, that are not needed

def error(loc, ij, message):
    if ij is not None:
        i, j = ij
        print(f"{loc}:{i}:{j}: {message}")
    else:
        print(f"{loc}: {message}")


def all_files(*dirs):
    dirs = list(reversed(dirs)) # from left to right -> reversed
    while dirs:
        path = dirs.pop()
        for f in os.scandir(path):
            if f.is_dir():
                dirs.append(f.path)
            if f.is_file():
                yield f.path

def collect_exports(fn):
    PRFX = "// export "
    c = Path(fn).read_text()
    tokens = {t for t, loc in identifier_tokens_filtered(c)}
    for i, l in enumerate(c.splitlines()):
        if l.startswith(PRFX):
            l = l[len(PRFX):].strip().split()
            if l[0] == "from":
                for imp in l[1:]:
                    yield from collect_exports(from_include_path(imp))
                continue
            for imp in l:

                yielded = False
                for t in tokens:
                    if fnmatch(t, imp):
                        yield t
                        yielded = True
                if not yielded:
                    error(fn, (i+1,1), f"pattern does not match: {imp}")


def collect_all_exports(files):
    exports = {}
    for fn in files:
        for e in collect_exports(fn):
            include_fn = to_include_path(fn)
            if e in exports:
                exports[e].add(include_fn)
            else:
                exports[e] = {include_fn}
    return exports

def find_next(str, pos, patterns):
    f_min = -1
    pat = None
    for p in patterns:
        f = str.find(p, pos)
        if f == -1: continue
        if pat is None or f < f_min:
            f_min = f
            pat = p

    return f_min, pat

def advance(ij, string):
    i, j = ij
    if "\n" in string:
        i += string.count("\n")
        j = string.rfind("\n")+1
    else:
        j += len(string)
    return i, j

def advance_i(ij, linecount):
    i, j = ij
    if linecount != 0:
        i += linecount
        j = 1
    return i, j

def advance_j(ij, rowcount):
    i, j = ij
    return i, j+rowcount

def remove_comments_seg(c):
    ij = 1,1

    p = 0
    while True:
        f, pat = find_next(c, p, ["/*", "//"])
        if f == -1: break

        yield c[p:f] + " ", ij
        pp = p
        if pat == "/*":
            p = c.find("*/", f+2)
            assert p != -1
            p+=2
        else:
            p = c.find("\n", f+2)
            if p == -1: p = len(c)
        ij = advance(ij, c[pp:p])

    yield c[p:], ij

def split_lines_seg(segments):
    for s, ij in segments:
        for i, l in enumerate(s.splitlines()):
            yield l, advance_i(ij, i)

def identifier_tokens(c):
    c_seg = remove_comments_seg(c)

    for l, ij in split_lines_seg(c_seg):
        w = ""
        for j, c in enumerate(l):
            w2 = w+c
            if w2.isidentifier():
                w = w2
            else:
                if w: yield (w, advance_j(ij, j-len(w)))
                w = ""
        if w: yield (w, advance_j(ij, j-len(w)+1))

def identifier_tokens_filtered(c):
    tokens = {}
    for t, loc in identifier_tokens(c):
        if len(t) >= 4 and t[3] == "_" and t[:3] in FILTER_NAMESPACES:
            if t not in tokens:
                tokens[t] = loc
    return tokens.items()

def from_include_path(fn):
    return "include/" + fn

def to_include_path(fn):
    if fn.startswith("include/"):
        return fn[8:]
    return fn

def extract_includes(ctx_error, c):
    PRFX = "#include \""
    includes = set()
    for i, l in enumerate(c.splitlines()):
        if not l.startswith(PRFX): continue

        l = l[len(PRFX):l.find("\"", len(PRFX))]
        if l in includes:
            ctx_error((i+1,1), f"duplicate include: {l}")
        else:
            includes.add(l)

    return includes

def get_headers(t, exports):
    return exports.get(t, set())

def validate_token(ctx_error, t, ij, exports, includes):
    headers = get_headers(t, exports)
    if not headers: return set()
    ok_includes = headers & set(includes)
    if not ok_includes:
        if MISSING_IMPORTS:
            ctx_error(ij, f"missing import for {t}: {' '.join(headers)}")
    return ok_includes

def corresponding_header(src_file):
    return src_file.replace("src/", "include/").replace(".c", ".h")

def validate_imports(fn, exports, src_includes):
    def ctx_error(ij, msg):
        error(fn, ij, msg)
    c = Path(fn).read_text()
    includes = extract_includes(ctx_error, c)
    parent_fn = to_include_path(corresponding_header(fn))
    parent_includes = set(src_includes.get(parent_fn, []))
    unnecessary_includes = includes & parent_includes - {parent_fn}
    if unnecessary_includes:
        error(fn, None, f"unnecessary includes: {' '.join(sorted(unnecessary_includes))}")
    if fn.endswith(".c") and not fn.endswith(".t.c") and not fn.endswith("main.c") and parent_fn not in includes:
        error(fn, None, f"missing own header include {parent_fn}")

    #inherit includes from header file
    includes |= parent_includes
    includes |= {to_include_path(fn)}

    ok_includes = set()
    for t, ij in identifier_tokens_filtered(c):
        ok_includes |= validate_token(ctx_error, t, ij, exports, includes)

    unused_includes = (includes - ok_includes - parent_includes) & set(exports.keys())
    if unused_includes and UNUSED_IMPORTS:
        error(fn, None, f"unused includes: {' '.join(sorted(unused_includes))}")

    return includes


def validate_all_imports(files, exports):
    imports = {}
    for fn in files:
        imports[to_include_path(fn)] = validate_imports(fn, exports, imports)



if __name__ == "__main__":
    exports = collect_all_exports(all_files("include"))
    validate_all_imports(all_files("include", "src"), exports)

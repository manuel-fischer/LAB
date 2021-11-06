from collections import namedtuple
from dataclasses import dataclass
import string
import os
import sys
import shutil
from weakref import ref as wref
from typing import TypeVar
import json

T = TypeVar('T')
def deref(w : wref[T]) -> T:
    return w() if w is not None else None

LANG = "cpp"
SRC_PATH = "include"
#Entity = namedtuple("Entity", """
#    type
#    category
#    brief
#    display
#    sections
#    childs
#""")
@dataclass
class Entity:
    type     : str
    category : str
    brief    : str
    display  : str
    sections : list[str]
    childs   : list['Entity']
    location : str
    xmd_location : tuple[str, int]
    src_location : tuple[str, int]
    config   : set[str]
    prev     : wref['Entity'] # or None
    next     : wref['Entity'] # or None

EntityType = namedtuple("EntityType", "title header_format")

HEADER_BRIEF     = 0
HEADER_SIGNATURE = 1
HEADER_TITLE     = 2

# TODO correct link generation with nested directories
#SEP = "/"  # nested directory structure
SEP = "--" # flat directory structure

X = EntityType
ENTITY_WORDS = {# Title                  Header format
    "topic":    X("Topic(s)",            HEADER_TITLE), # TODO parse whole line as title

    "file":     X("File(s)",             HEADER_BRIEF),
    "directory":X("Director(ies)y",      HEADER_BRIEF),
    "module":   X("Module(s)",           HEADER_BRIEF),
    "namespace":X("Namespace(s)",        HEADER_BRIEF),
    
    "fn":       X("Function(s)",         HEADER_SIGNATURE),
    "class":    X("Class(es)",           HEADER_SIGNATURE),
    "struct":   X("Structure(s)",        HEADER_SIGNATURE),
    "type":     X("Type(s)",             HEADER_BRIEF),
    "const":    X("Constant(s)",         HEADER_BRIEF),
    "var":      X("Variable(s)",         HEADER_BRIEF),
    "def":      X("Macro Definition(s)", HEADER_SIGNATURE),
    
    "param":    X("Parameter(s)",        HEADER_BRIEF),
    "attr":     X("Attribute(s)",        HEADER_BRIEF),
    "retval":   X("Return Value(s)",     HEADER_BRIEF),
    #"example":  X("Example(s)",          HEADER_TITLE),

    # Entities that are not necessarily a physical part of source code
    # but more specific entities provided by the documented interface
    "cmd":      X("Command(s)",          HEADER_BRIEF),
    "tag":      X("Tag(s)",              HEADER_BRIEF),
    "opt":      X("Option(s)",           HEADER_BRIEF),
    "elem":     X("Element(s)",          HEADER_BRIEF),
}
del X


SPECIAL_SECTIONS = {
    "syn":          "**Synopsis**\n", # if @syn(n), then multiple overloads could be written
                                      # if the syntax of a declaration is clear, the entity
                                      # itself can be introduced with the syn
    "default":      "# Default Value\n",
    "return":       "# Return Value\n",
    "sidefx":       "# Side Effects\n",
    "pre":          "# Preconditions\n",
    "post":         "# Postconditions\n",
    "todo":         "# TODO\n",
    "see":          "# See also\n",
    "note":         "**Note**  \n",
    "description":  "",
    "example":      "# Example\n",
}


SECTION_ORDER = """
    syn
    description
    note

    param

    opt
    cmd
    tag
    elem
    
    topic
    directory
    file
    module
    namespace
    
    const
    var
    attr

    default
    pre
    post
    return
    retval
    sidefx
    
    type
    struct
    class
    fn
    def
    
    example

    see

    todo
""".split()


COMMANDS = {}
def command(name):
    def command2(func):
        COMMANDS[name] = func
    return command2

@command("brief")
def cmd_brief(e : Entity, content : str):
    e.brief = (e.brief + " " + content).strip()

@command("briefx")
def cmd_briefx(e : Entity, content : str):
    e.brief = (e.brief + " " + content).strip()
    e.sections["description"] += content+"\n"

@command("disp")
def cmd_disp(e : Entity, content : str):
    e.display = content.strip()

@command("config")
def cmd_config(e : Entity, content : str):
    for arg in content.split():
        if arg[0] == "!":
            e.config.remove(arg[1:])
        else:
            e.config.add(arg)

@command("locate")
def cmd_locate(e : Entity, content : str):
    if content == "auto":
        ... # TODO auto detect file and line
    else:
         # TODO check syntax
        sp = content.split()
        if len(sp) == 1:
            file = sp[0]
            line = None
        elif len(sp) >= 2:
            file = sp[0]
            line = int(sp[1])
        e.src_location = (file, line)

INDENT_WIDTH = 4

Token = namedtuple("Token", "pos text")

def split_tokens(txt):
    tokens = []
    cur_start = 0
    cur_token = ""
    for i, c in enumerate(txt):
        if c in string.whitespace:
            if cur_token:
                tokens.append(Token(cur_start, cur_token))
            cur_start = i+1
            cur_token = ""
        elif c in "()[]{}":
            if cur_token:
                tokens.append(Token(cur_start, cur_token))
            cur_start = i+1
            cur_token = ""
                
            tokens.append(Token(i, c))
        else:
            cur_token += c

    if cur_token:
        tokens.append(Token(cur_start, cur_token))
    return tokens

def text_slice(tokens, index, end=None):
    if index >= len(tokens): return slice(-1,-1)
    else:
        first = tokens[index].pos
        last  = None if end is None or end >= len(tokens) else tokens[end].pos
        return slice(first,last)

ANCHOR_CHARS = set(" -_" + string.ascii_letters + string.digits)

def to_md_anchor(txt):
    return "".join(
        '-' if c == ' ' else c.lower()
        for c in txt
        if c in ANCHOR_CHARS
    )

def to_md_filename_part(name):
    return to_md_anchor(name)


def make_loc(path):
    return path.replace("/",  SEP) \
               .replace("\\", SEP)


def correct_grammar(string, amount):
    l = string.rfind("(")
    if l == -1: return string
    r = string.rfind(")")
    if amount == 1:
        return string[:l] + string[r+1:]
    else:
        return string[:l] + string[l+1:r]


def signature2display(signature):
    tokens = split_tokens(signature)
    if (i := first_group(tokens)) != None:
        return tokens[i-1].text
    return tokens[0].text


def entity_has_subfile(entity):
    return bool(entity.childs) \
        or bool("".join(entity.sections.values()).strip())


# Parsing


def dedent(line):
    if line[:1] == '\t': return line[1:]
    if line[:INDENT_WIDTH].replace(" ", "") == "": return line[INDENT_WIDTH:]
    return None

def parse_block(lines):
    block = []
    for l in lines:
        if (d := dedent(l)) is not None:
            block.append(d)
        else:
            break
    return (block, lines[len(block):])

def first_group(tokens, start=0):
    for i in range(start, len(tokens)):
        if tokens[i].text in "()[]{}":
            return i
    return None

def parse_group(tokens, start=0, group="[]"):
    if start < len(tokens) and tokens[start].text == group[0]:
        depth = 0
        for i in range(start+1, len(tokens)):
            if   tokens[i].text == group[1]: depth -= 1
            elif tokens[i].text == group[0]: depth += 1

            if depth == -1: break
        else:
            raise Exception(f"Unmatched {group[0]}")

        return (start+1, i, i+1)            
    else:
        return (start, start, start)

def pack_code(code):
    return f"```{LANG}\n{code}\n```"

def parse_xmd(xmd_lines, e : Entity, line_no=1) -> Entity:
    if "description" not in e.sections: e.sections["description"] = ""

    try:
        while xmd_lines:
            l = xmd_lines[0]
            if l.startswith("@"):
                block, xmd_lines = parse_block(xmd_lines[1:])
                
                is_self = l.startswith("@@")
                    
                ll = l[(1, 2)[is_self]:]
                tokens = split_tokens(ll)
                tag = tokens[0].text
                if tag in ENTITY_WORDS:
                    if is_self:
                        e_location = e.location
                    else:
                        e_location = None

                    e_type = tag
                    first, last, rest = parse_group(tokens, 1, "[]")
                    e_category = ll[text_slice(tokens, first, last)] if first!=last else ""
     
                    head_fmt = ENTITY_WORDS[tag].header_format
                    e_signature = None
                    if head_fmt == HEADER_BRIEF:
                        e_display = tokens[rest].text
                        e_display = f"`{e_display}`"
                        e_brief = ll[text_slice(tokens, rest+1)]
                    elif head_fmt == HEADER_SIGNATURE:
                        e_signature = ll[text_slice(tokens, rest)]
                        e_display = signature2display(e_signature)
                        if e_signature == e_display: e_signature = None
                        e_display = f"`{e_display}`"
                        e_brief = ""
                    elif head_fmt == HEADER_TITLE:
                        e_display = ll[text_slice(tokens, rest)] 
                        e_brief = ""

                    e_sections = {"syn": pack_code(e_signature)} if e_signature is not None else {}
                        
                    sub_entity = parse_xmd(
                        block,
                        Entity(
                            type = e_type,
                            category = e_category,
                            brief = e_brief,
                            display = e_display,
                            sections = e_sections,
                            childs = [],
                            location = e_location, # TODO
                            xmd_location = (e.xmd_location[0], line_no),
                            src_location = e.src_location,
                            config = set(),
                            prev = None,
                            next = None,
                        ),
                        line_no
                    )
                    if is_self:
                        e = sub_entity # TODO: do not override everything
                    else:
                        e.childs.append(sub_entity)
                else:
                    if is_self:
                        raise Exception(f"Self referring syntax not allowed with {tag}")
                    content = "\n".join([ll[text_slice(tokens, 1)]]+block)
                    if tag in SPECIAL_SECTIONS or tag == "syn*":
                        if tag == "syn": content = pack_code(content)
                        if tag == "syn*": tag = "syn"
                        try:             e.sections[tag] += content+"\n"
                        except KeyError: e.sections[tag]  = content+"\n"
                    elif tag in COMMANDS:
                        COMMANDS[tag](e, content)
                    else:
                        raise Exception(f"Unexpected command {tag}")
                        
            
            else:
                block, xmd_lines = [], xmd_lines[1:]
                # just markdown text, in the description
                e.sections["description"] += l+"\n"
            line_no += 1+len(block)
    except:
        print(f"Error at line {line_no}")
        raise

    return e

LONG_LINE = "&#8213;"
SPLIT_LINE = "|"

LARGE_SPACE = '&nbsp;'*3

DIRECTION_STR = {
    "left":  "&#8592;",
    "up":    "&#8593;",
    "right": "&#8594;",
    "down":  "&#8595;",
}

def str_join_nonempty(join_string, l):
    return join_string.join(e for e in l if e.strip())

def generate_browse_link(direction, file_entity):
    if file_entity is None: return ""
    assert type(file_entity) == Entity
    return f"[{DIRECTION_STR[direction]} {file_entity.display}]({file_entity.location})"
    

def generate_browse(parent, file_entity):
    md = str_join_nonempty(f"{LARGE_SPACE}{SPLIT_LINE}{LARGE_SPACE}", [
        generate_browse_link("left", deref(file_entity.prev)),
        generate_browse_link("up", parent),
        generate_browse_link("right", deref(file_entity.next))
    ])
    if file_entity.xmd_location:
        if file_entity.xmd_location[1] is None:
            xmd_loc = f"../xdoc/{file_entity.xmd_location[0]}"
        else:
            xmd_loc = f"../xdoc/{file_entity.xmd_location[0]}#L{file_entity.xmd_location[1]}"
        md = str_join_nonempty(f"{LARGE_SPACE*2}{SPLIT_LINE*2}{LARGE_SPACE*2}", [
            md,
            f"<small>[\\* xdoc]({xmd_loc})</small>"
        ])
    return md + "\n"


def realize_filestructure(xmd_entity : Entity, filename : str, depth : int = 999, section_depth : int = 1):
    assert filename
    if not xmd_entity.location:
        xmd_entity.location = filename
    else:
        filename = xmd_entity.location
    file_prefix = os.path.splitext(filename)[0]

    prev = None
    for sect in SECTION_ORDER:
        if sect in ENTITY_WORDS:
            s_childs = [c for c in xmd_entity.childs if c.type == sect]
            s_childs = sorted(s_childs, key=lambda c: c.category or "")

            s_childs_exist = list(filter(entity_has_subfile, s_childs))

            s_files = [f"{file_prefix}{SEP}{to_md_filename_part(c.display)}.md" for c in s_childs_exist]

            for i, c in enumerate(s_childs_exist):
                if prev is not None:
                    prev.next = wref(c)
                    c.prev = wref(prev)
                prev = c

                if depth <= 0:
                    pass
                else:
                    realize_filestructure(c, filename = s_files[i], depth=depth-1, section_depth=1)



def xmd2md(xmd_entity : Entity, parent_entity : Entity, depth : int = 999, section_depth : int = 1):
    filename = xmd_entity.location
    #file_prefix = os.path.splitext(filename)[0]
    assert filename
    
    subfiles = []
    md = ""
    md += generate_browse(parent_entity, xmd_entity)
    md += "***\n\n"
    md += section_depth*"#" + f" {xmd_entity.display}\n"
    if xmd_entity.category or xmd_entity.type in ENTITY_WORDS or xmd_entity.src_location:
        md += "<small>"
        if xmd_entity.type in ENTITY_WORDS:
            md += f"*{correct_grammar(ENTITY_WORDS[xmd_entity.type].title, 1)}*"
        if xmd_entity.type and xmd_entity.category:
            md += " &nbsp; - &nbsp; "
        if xmd_entity.category:
            md += f"**{xmd_entity.category}**"

        if (xmd_entity.type or xmd_entity.category) and xmd_entity.src_location:
            md += " &nbsp; - &nbsp; "
        if xmd_entity.src_location:
            file, line = xmd_entity.src_location
            if SRC_PATH:
                src_link = f"{SRC_PATH}/{file}"
            else:
                src_link = file
            if line is not None:
                src_link = f"{src_link}#L{line}"
            #md += f" &nbsp;&nbsp;&nbsp;&gt; "
            md += f"[\"{file}\"](../{src_link})"
        md += "</small>  \n"
        
    for sect in SECTION_ORDER:
        if sect in ENTITY_WORDS:
            s_childs : list[Entity]
            s_childs = [c for c in xmd_entity.childs if c.type == sect]
            s_childs = sorted(s_childs, key=lambda c: c.category or "")

            s_childs_exist = list(filter(entity_has_subfile, s_childs))

            #s_files = [(f"{file_prefix}--{to_md_filename_part(c.display)}.md", c.display) for c in s_childs_exist]
            
            if not s_childs: continue
            section_title = correct_grammar(ENTITY_WORDS[sect].title, len(s_childs))
            md += section_depth*"#" + "# " + section_title + "\n"
            
            cur_category = ""
            for c in s_childs:
                if c.category != cur_category:
                    md += f"<small>**{c.category}**</small>  \n"
                    cur_category = c.category


                def make_link(c):
                    link = f"{c.display}"

                    if entity_has_subfile(c):
                        if depth <= 0:
                            link = f"[{link}](#{to_md_anchor(c.display)})"
                        else:
                            assert c.location
                            link = f"[{link}]({c.location})"
                        
                    return f"**{link}**"

                md += make_link(c)
                
                if c.brief:
                    md += f" {LONG_LINE} {c.brief}"

                #md += "  \n" # two spaces -- linebreak

                # Add inline childs
                #for cc in c.childs:
                #    if "inline" in cc.config:
                #        md += f"&nbsp;&nbsp;&nbsp; {link_brief(cc)}"
                #        md += "  \n" # two spaces -- linebreak

                has_inline_childs = False
                has_more = False
                # Add inline childs
                for cc in c.childs:
                    if "inline" in cc.config:
                        if not has_inline_childs:
                            #md += f"&nbsp;&nbsp;&nbsp; <small>&gt;"
                            md += f"&nbsp; &gt;&nbsp; <small>"
                            has_inline_childs = True
                        else:
                            md += " &bull; "
                        md += make_link(cc)
                    else:
                        has_more = True

                if has_inline_childs:
                    if has_more:
                        md += " ..."
                    md += "</small>"
                    #md += "  \n" # two spaces -- linebreak
                md += "  \n" # two spaces -- linebreak

                
            for c in s_childs_exist:
                if depth <= 0: # and "inline" not in c.config:
                    subfiles += xmd2md(c, parent_entity = xmd_entity, depth=-1, section_depth=section_depth+2)
                else:
                    subfiles += xmd2md(c, parent_entity = xmd_entity, depth=max(-1, depth-1), section_depth=1)
                
        else:
            sect_md = xmd_entity.sections.get(sect, "")
            if sect_md:
                section_title = SPECIAL_SECTIONS[sect]
                md += section_title.replace("#", section_depth*"#"+"#") + "\n"
                md += sect_md + "\n"

    if subfiles and depth == 0:
        md += section_depth*"#" + "# " + correct_grammar("Child(s)", len(subfiles)) + "\n"
        first = True
        for e_fn, e_md in subfiles.items():
            if first:
                first = False
            else:
                md += "***\n"
            #md += section_depth*"#" + f"# `{e_fn}`\n"
            md += e_md
            
        subfiles = []

    subfiles += [(xmd_entity.location, xmd_entity.display, md)]
    return subfiles

def read_file(fname):
    print(f"Reading {fname}")
    with open(fname, "rt") as f:
        s = f.read()
    return s

def write_file(fname, s):
    path = os.path.dirname(fname)
    if not os.path.isdir(path):
        print(f"Create {path} for {fname}")
        os.makedirs(path, exist_ok=True)
        
    print(f"Writing {fname}")
    with open(fname, "wt") as f:
        f.write(s)

def parse_xmd_file(cwd, ifile):
    xmd_ifile = os.path.join(cwd,"xdoc",ifile)
    xmd_src = read_file(xmd_ifile)
    ofile = make_loc(os.path.splitext(ifile)[0]+".md")
    return parse_xmd(
        xmd_src.split("\n"),
        Entity(
            type = "file",  
            category = "",
            brief = "",
            display = f"`{os.path.splitext(os.path.split(xmd_ifile)[-1])[0]}`",
            sections = {},
            childs = [],
            location = ofile,
            xmd_location = (ifile, 1),
            src_location = None,
            config = set(),
            prev = None,
            next = None,
        )
    )

def generate_md_files(cwd, root, entity):
    file_contents = xmd2md(
        entity,
        root,
        depth=999
    )

    for fn, display, md in file_contents:
        write_file(os.path.join(cwd,"doc",fn), md)


def load_node(cwd, p, is_root=False):
    print(f"Entering {p}{os.sep}")
    file_entities = []
    #ifiles = sorted(os.listdir(os.path.join(cwd, "xdoc", p)))
    #... [parse_xmd_file(cwd, ifile) for ifile in ifiles]
    P = os.path.join(cwd, "xdoc", p)
    ifiles = sorted(os.listdir(P))
    for f in ifiles:
        fn, ext = os.path.splitext(f)
        
        if os.path.isdir(os.path.join(P, f)): # load as directory
            print(f"Directory {f}")
            file_entities.append(load_node(cwd, os.path.join(p, f)))
        elif os.path.exists(os.path.join(P, fn)):
            print(f"Ignore {f}")
            pass # already handled in previous case
        elif ext != ".xmd":
            print(f"Ignore {f}")
            # Other files without .xmd extension
        else:
            print(f"File {f}")
            file_entities.append(parse_xmd_file(cwd, os.path.join(p, f)))
            

    if p == "": # root
        root = Entity(
            type = "table",
            category = "",
            brief = "",
            display = "Table",
            sections = {},
            childs = [],
            location = "table.md",
            xmd_location = None,
            src_location = None,
            config = set(),
            prev = None,
            next = None
        )
    elif os.path.exists(os.path.join(cwd, "xdoc", p+".xmd")):
        root = parse_xmd_file(cwd, p+".xmd")
    else:
        root = Entity(
            type = "directory",
            category = "",
            brief = "",
            display = f"`{p}`",
            sections = {},
            childs = [],
            location = make_loc(p+".md"),
            xmd_location = (p, None),
            src_location = None,
            config = set(),
            prev = None,
            next = None
        )

    root.childs += file_entities

    #print(root)

    return root
    #return file_entities[0] if file_entities else root


def process_doc(cwd):
    try:
        with open(os.path.join(cwd,"xdoc","config.json")) as f:
            config = json.loads(f.read())

    except FileNotFoundError:
        config = {}
    
    global LANG, SRC_PATH
    if "lang" in config: LANG = config["lang"]
    if "src-path" in config: SRC_PATH = config["src-path"]

    table_ofile = os.path.join(cwd,"doc","table.md")
    ifiles = sorted(os.listdir(os.path.join(cwd, "xdoc")))

    print(f"Cleaning doc{os.sep} directory")
    try:
        shutil.rmtree(os.path.join(cwd, "doc"))
    except FileNotFoundError: pass
    #if os.path.isdir(os.path.join(cwd, "doc")):
    #    for f in os.listdir(os.path.join(cwd, "doc")):
    #        delete_file(os.path.join(cwd, "doc", f))

    #file_entities = [parse_xmd_file(cwd, ifile) for ifile in ifiles]

    root = load_node(cwd, "", is_root=True)
    realize_filestructure(root, "table.md")
    generate_md_files(cwd, None, root)

    write_file("doc/.gitattributes", "* -diff")

if __name__ == "__main__":
    cwd = sys.argv[1] if len(sys.argv) == 2 else "."
    process_doc(cwd)

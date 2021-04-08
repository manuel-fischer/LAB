from collections import namedtuple
import string
import os
import sys

LANG = "cpp"
Entity = namedtuple("Entity", """
    type
    category
    brief
    display
    sections
    childs
""")

EntityType = namedtuple("EntityType", "title inline_brief")

X = EntityType
ENTITY_WORDS = {# Title                  Has inline brief
    "file":     X("File(s)",             True),
    "module":   X("Module(s)",           True),
    "namespace":X("Namespace(s)",        True),
    
    "fn":       X("Function(s)",         False),
    "class":    X("Class(es)",           False),
    "struct":   X("Structure(s)",        False),
    "type":     X("Type(s)",             True),
    "const":    X("Constant(s)",         True),
    "var":      X("Variable(s)",         True),
    "def":      X("Macro Definition(s)", False),
    
    "param":    X("Parameter(s)",        True),
    "attr":     X("Attribute(s)",        True),
    "retval":   X("Return Value(s)",     True),
    #"example":  X("Example(s)",          False),

    # Entities that are not necessarily a physical part of source code
    # but more specific entities provided by the documented interface
    "cmd":      X("Command(s)",          False),
    "tag":      X("Tag(s)",              True),
    "opt":      X("Option(s)",           True),
    "elem":     X("Element(s)",          True),
}
del X

SECTION_ORDER = """
    syn
    description
    note

    param

    opt
    cmd
    tag
    elem
    
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


SPECIAL_SECTIONS = {
    "syn": "**Synopsis**\n", # if @syn(n), then multiple overloads could be written
                             # if the syntax of a declaration is clear, the entity
                             # itself can be introduced with the syn
    "default": "# Default Value\n",
    "return": "# Return Value\n",
    "sidefx": "# Side Effects\n",
    "pre":  "# Preconditions\n",
    "post":  "# Postconditions\n",
    "todo": "# TODO\n",
    "see": "# See also\n",
    "note": "**Note**  \n",
    "description": "",
    "example":  "# Example\n",
}


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

def correct_grammar(string, amount):
    if amount == 1:
        return string[:string.rfind("(")]
    else:
        return string.replace("(","") \
                     .replace(")","")


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

def parse_xmd(xmd_lines, proto, line_no=1):
    (
        type,
        category,
        brief,
        display,
        sections,
        childs
    ) = proto

    if "description" not in sections: sections["description"] = ""

    try:
        while xmd_lines:
            l = xmd_lines[0]
            if l.startswith("@"):
                block, xmd_lines = parse_block(xmd_lines[1:])
                
                ll = l[1:]
                tokens = split_tokens(ll)
                tag = tokens[0].text
                if tag in ENTITY_WORDS:
                    e_type = tag
                    first, last, rest = parse_group(tokens, 1, "[]")
                    e_category = ll[text_slice(tokens, first, last)] if first!=last else ""
     
                    if ENTITY_WORDS[tag].inline_brief:
                        e_signature = e_display = tokens[rest].text
                        e_brief = ll[text_slice(tokens, rest+1)]
                    else:
                        e_signature = ll[text_slice(tokens, rest)]
                        e_display = signature2display(e_signature)
                        e_brief = ""
                    e_sections = {"syn": pack_code(e_signature)} if e_signature != e_display else {}
                        
                    entity = parse_xmd(
                        block,
                        Entity(
                            type = e_type,
                            category = e_category,
                            brief = e_brief,
                            display = e_display,
                            sections = e_sections,
                            childs = []
                        ),
                        line_no
                    )

                    childs.append(entity)
                else:
                    content = "\n".join([ll[text_slice(tokens, 1)]]+block)
                    if tag == "brief":
                        brief = (brief + " " + content).strip()
                    elif tag == "briefx":
                        brief = (brief + " " + content).strip()
                        sections["description"] += content+"\n"
                    elif tag == "disp":
                        display = content.strip()
                    elif tag in SPECIAL_SECTIONS or tag == "syn*":
                        if tag == "syn": content = pack_code(content)
                        if tag == "syn*": tag = "syn"
                        try:             sections[tag] += content+"\n"
                        except KeyError: sections[tag]  = content+"\n"
                    else:
                        raise Exception(f"Unexpected command {tag}")
                        
            
            else:
                block, xmd_lines = [], xmd_lines[1:]
                # just markdown text, in the description
                sections["description"] += l+"\n"
            line_no += 1+len(block)
    except:
        print(f"Error at line {line_no}")
        raise

    return Entity(
        type,
        category,
        brief,
        display,
        sections,
        childs
    )

LONG_LINE = "&#8213;"
SPLIT_LINE = "|"

DIRECTION_STR = {
    "left":  "&#8592;",
    "up":    "&#8593;",
    "right": "&#8594;",
    "down":  "&#8595;",
}

def str_join_nonempty(join_string, l):
    return join_string.join(e for e in l if e.strip())

def generate_browse_link(direction, file):
    assert type(file) == tuple
    return f"[{DIRECTION_STR[direction]} {file[1]}]({file[0]})"
    

def generate_browse(parent, files, i):
    return str_join_nonempty(f" {SPLIT_LINE} ", [
        (generate_browse_link("left", files[i-1]) if i != 0 else ""),
        (generate_browse_link("up", parent)),
        (generate_browse_link("right", files[i+1]) if i != len(files)-1 else "")
    ]) + "\n"


def xmd2md(xmd_entity, parent_file, files, file_index, depth=999, section_depth=1):
    file = files[file_index]
    filename = file[0]
    file_prefix = os.path.splitext(filename)[0]
    
    subfiles = []
    md = ""
    md += generate_browse(parent_file, files, file_index)
    md += "***\n\n"
    md += section_depth*"#" + f" `{xmd_entity.display}`\n"
    for sect in SECTION_ORDER:
        if sect in ENTITY_WORDS:
            s_childs = [c for c in xmd_entity.childs if c.type == sect]
            s_childs = sorted(s_childs, key=lambda c: c.category or "")

            s_childs_exist = list(filter(entity_has_subfile, s_childs))

            s_files = [(f"{file_prefix}--{to_md_filename_part(c.display)}.md", c.display) for c in s_childs_exist]
            
            if not s_childs: continue
            section_title = correct_grammar(ENTITY_WORDS[sect].title, len(s_childs))
            md += section_depth*"#" + "# " + section_title + "\n"
            
            cur_category = ""
            i = 0
            for c in s_childs:
                if c.category != cur_category:
                    md += f"<small>**{c.category}**</small>  \n"
                    cur_category = c.category

                link = f"`{c.display}`"

                if entity_has_subfile(c):
                    if depth <= 0:
                        link = f"[{link}](#{to_md_anchor(c.display)})"
                    else:
                        link = f"[{link}]({s_files[i][0]})"
                    i+=1
                    
                md += f"**{link}**" # linebreak
                if c.brief:
                    md += f" {LONG_LINE} {c.brief}"
                md += "  \n" # two spaces -- linebreak
                
            for i, c in enumerate(s_childs_exist):
                kw = dict(parent_file = file, files = s_files, file_index = i)
                if depth <= 0:
                    subfiles += xmd2md(c, **kw, depth=-1, section_depth=section_depth+2)
                else:
                    subfiles += xmd2md(c, **kw, depth=depth-1, section_depth=1)
                
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

    subfiles += [(filename, xmd_entity.display, md)]
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

def delete_file(fname):
    print(f"Deleting {fname}")
    os.remove(fname)

def parse_xmd_file(cwd, ifile):
    xmd_ifile = os.path.join(cwd,"xdoc",ifile)
    xmd_src = read_file(xmd_ifile)
    return parse_xmd(
        xmd_src.split("\n"),
        Entity(
            type = "file",  
            category = "",
            brief = "",
            display = os.path.splitext(os.path.split(xmd_ifile)[-1])[0],
            sections = {},
            childs = []
        )
    )

def generate_md_files(cwd, o_files, file_index, entity):
    file_contents = xmd2md(
        entity,
        ("table.md", "table"),
        o_files,
        file_index,
        depth=999
    )

    for fn, display, md in file_contents:
        write_file(os.path.join(cwd,"doc",fn), md)
        

def process_doc(cwd):
    table_ofile = os.path.join(cwd,"doc","table.md")
    ifiles = sorted(os.listdir(os.path.join(cwd, "xdoc")))
    ofiles = [os.path.splitext(f)[0]+".md" for f in ifiles]

    if os.path.isdir(os.path.join(cwd, "doc")):
        for f in os.listdir(os.path.join(cwd, "doc")):
            delete_file(os.path.join(cwd, "doc", f))

    file_entities = [parse_xmd_file(cwd, ifile) for ifile in ifiles]
    ofiles_tup = [(f, e.display) for f, e in zip(ofiles, file_entities)]

    for i, f in enumerate(ofiles):
        generate_md_files(cwd, ofiles_tup, i, file_entities[i])

    # file table
    md = ""
    md += "## Files\n"
    for i, f in enumerate(ofiles_tup):
        file, disp = f
        md += f"{i}. [{disp}]({file})\n"


    write_file(table_ofile, md)

if __name__ == "__main__":
    cwd = sys.argv[1] if len(sys.argv) == 2 else "."
    process_doc(cwd)

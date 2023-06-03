import os
from typing import Callable, Dict, List, Tuple
from dataclasses import dataclass
from itertools import product

NameTok = str
TypeTok = str

# tuple -> #0 <name> #2
ComplexType : type = 'TypeTok | Tuple[str, str]'

UnaryTemplate = Callable
BinaryTemplate = Callable
TernaryTemplate = Callable
NaryTemplate = Callable

## Template Functions
def op1(tok : str) -> UnaryTemplate:
    return lambda a: f"({tok}({a}))"

def op2(tok : str) -> BinaryTemplate:
    return lambda a, b: f"(({a}) {tok} ({b}))"

def fun1(func : str) -> UnaryTemplate:
    return lambda a: f"{func}({a})"

def fun2(func : str) -> BinaryTemplate:
    return lambda a, b: f"{func}({a}, {b})"

def fun3(func : str) -> TernaryTemplate:
    return lambda a, b, c: f"{func}({a}, {b}, {c})"

def funN(n : int, func : str) -> TernaryTemplate:
    fmt = f"{func}({', '.join(f'{{{i}}}'for i in range(n))})"
    return fmt.format

def cast1(type : ComplexType) -> UnaryTemplate:
    if isinstance(type, tuple):
        t0, t1 = type
        return lambda a: f"({t0}{t1})({a})"
    else:
        return lambda a: f"({type})({a})"

def nary_reduce(func : BinaryTemplate) -> NaryTemplate:
    def fn(*e):
        if len(e) == 1: return e[0]
        split = (len(e)+1) // 2
        left = fn(*e[:split])
        right = fn(*e[split:])
        return func(left, right)
    return fn

def overload(temp : NaryTemplate, overloads : Dict[int, NaryTemplate]):
    def fn(*e):
        for argc, overload in overloads.items():
            if len(e) == argc: return overload(*e)
        return temp(*e)
    return fn

def nary_reduce_2_3(binary : BinaryTemplate, ternary : TernaryTemplate):
    return overload(nary_reduce(binary), {3:ternary})


def nary_new(type : str) -> NaryTemplate:
    return lambda *e: f"({type}) {{ {', '.join(e)} }}"

def self_adjoint(op : BinaryTemplate) -> UnaryTemplate:
    return lambda a: op(a, a)

## End Template Functions


## Template Meta-Functions

def map_components(component_wise : NaryTemplate, fields : List[NameTok], vectors : List[NameTok]):
    return [
        component_wise(*(f"{v}.{f}" for v in vectors)) for f in fields
    ]

def components(fields : List[NameTok], vector : NameTok):
    return [f"{vector}.{f}" for f in fields]

## End Template Meta-Functions

MIN_NARY = nary_reduce_2_3(fun2("LAB_MIN"), fun3("LAB_MIN3"))
MAX_NARY = nary_reduce_2_3(fun2("LAB_MAX"), fun3("LAB_MAX3"))
MIN_INDEX_NARY = overload(None, {
    2:fun2("LAB_V_MIN_INDEX2"),
    3:fun3("LAB_V_MIN_INDEX3"),
    4:funN(4, "LAB_V_MIN_INDEX4"),
})
MAX_INDEX_NARY = overload(None, {
    2:fun2("LAB_V_MAX_INDEX2"),
    3:fun3("LAB_V_MAX_INDEX3"),
    4:funN(4, "LAB_V_MAX_INDEX4"),
})


## Type Functions

def const(type):
    assert not isinstance(type, tuple)

    if set(type) & set("*()[]"):
        return f"{type} const"

    return f"const {type}"

def ptr(type):
    if isinstance(type, tuple):
        t0, t1 = type
        return (f"{t0} (*", f"){t1}")

    return f"{type}*"

def cptr(type):
    return ptr(const(type))

def array(type, len):
    assert not isinstance(type, tuple)
    return (type, f"[{len}]")

def carray(type, len):
    return array(const(type), len)

## End Type Functions





HEADER = """
#pragma once

#include "LAB_math.h"
#include "LAB_select.h"
#include "LAB_direction.h"
#include "LAB_bits.h"
"""

HEADER_2 = """

#define LAB_V_MIN_INDEX2(x, y) LAB_SELECT_MIN(x,0, y,1)
#define LAB_V_MAX_INDEX2(x, y) LAB_SELECT_MAX(x,0, y,1)

#define LAB_VEC2_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y })

// export LAB_Vec2*
// export LAB_Box2*
"""

HEADER_3 = """
#define LAB_V_MIN_INDEX3(x, y, z) LAB_SELECT_MIN3(x,0, y,1, z,2)
#define LAB_V_MAX_INDEX3(x, y, z) LAB_SELECT_MAX3(x,0, y,1, z,2)

#define LAB_VEC3_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y, (ptr)->z })

// export LAB_Vec3*
// export LAB_Box3*
"""

HEADER_4 = """
#define LAB_V_MIN_INDEX4(x, y, z, w) LAB_SELECT_MIN3(x,0, y,1, LAB_MIN(z,w),LAB_SELECT_MIN(z,2, w,3))
#define LAB_V_MAX_INDEX4(x, y, z, w) LAB_SELECT_MAX3(x,0, y,1, LAB_MAX(z,w),LAB_SELECT_MAX(z,2, w,3))

#define LAB_VEC4_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y, (ptr)->z, (ptr)->w })

// export LAB_Vec4*
// export LAB_Box4*
"""

LAB_VEC_DECL = "LAB_INLINE LAB_ALWAYS_INLINE"
LAB_CONST = "LAB_VALUE_CONST"
LAB_PURE = "LAB_PURE"

LAB_VEC_DECL_CONST = f"{LAB_VEC_DECL} {LAB_CONST}"
LAB_VEC_DECL_PURE = f"{LAB_VEC_DECL} {LAB_PURE}"


ABS_OVERLOADS = {
    "int": "abs",
    "size_t": "",
    "float": "fabsf",
    "double": "fabs",
}


VEC2 = ["x","y"]
VEC3 = ["x","y","z"]
VEC4 = ["x","y","z","w"]


FILES = [
    ("LAB_vec2.h", VEC2, HEADER_2),
    ("LAB_vec3.h", VEC3, HEADER_3),
    ("LAB_vec4.h", VEC4, HEADER_4)
]

# type suffix, ctype, is_integer
TYPES = [
    ("I", "int", True),
    ("Z", "size_t", True),
    ("F", "float", False),
    ("D", "double", False),
]
TYPE_SUFFIXES = [suffix for suffix, _, _ in TYPES]

# from, to, suffix or None, component_func or None
CONVERSIONS = [
    ("I", "F", None, None),
    ("I", "D", None, None),
    ("F", "D", None, None),
    ("D", "F", "Cast", None),
    ("F", "I", "FastFloor", fun1("LAB_FastFloorF2I")),
    ("D", "I", "FastFloor", fun1("LAB_FastFloorD2I")),

    ("I", "Z", None, None),
    ("Z", "I", None, None),
    ("Z", "F", None, None),
    ("Z", "D", None, None),
]


class DefArgs:
    @property
    def namespace(self): ...

    def name(self, name : str):
        return f"{self.namespace}_{name}"


@dataclass
class VecDefArgs(DefArgs):
    vtype : TypeTok
    ctype : TypeTok
    fields : List[NameTok]
    is_integer : bool

    @property
    def is_float(self) -> bool: return not self.is_integer

    @property
    def namespace(self): return self.vtype

def call(v : DefArgs, opname : str, args : List[str]):
    return f"{v.namespace}_{opname}({', '.join(args)})"


def def_global_proc(name : NameTok, args : List[Tuple[TypeTok, NameTok]], ret : ComplexType, content : str, attrs=LAB_VEC_DECL):
    cargs = ", ".join(f"{type} {aname}" for type, aname in args)

    if isinstance(ret, tuple):
        t0, t1 = ret
        if t0[-1] not in "*()": t0 += " "
        yield f"{attrs} {t0}{name}({cargs}){t1} {{ {content} }}"
    else:
        yield f"{attrs} {ret} {name}({cargs}) {{ {content} }}"

def def_global_func(name : NameTok, args : List[Tuple[TypeTok, NameTok]], ret : ComplexType, expr : str, attrs=LAB_VEC_DECL_CONST):
    yield from def_global_proc(name, args, ret, content=f"return {expr};", attrs=attrs)


def def_proc(v : DefArgs, name : NameTok, args : List[Tuple[TypeTok, NameTok]], ret : ComplexType, content : str, attrs=LAB_VEC_DECL):
    yield from def_global_proc(f"{v.namespace}_{name}", args, ret, content, attrs)

def def_func(v : DefArgs, name : NameTok, args : List[Tuple[TypeTok, NameTok]], ret : ComplexType, expr : str, attrs=LAB_VEC_DECL_CONST):
    yield from def_global_func(f"{v.namespace}_{name}", args, ret, expr, attrs)




def def_global_struct(name : NameTok, fields : List[Tuple[TypeTok, List[NameTok]]]):
    fields = " ".join(
        f"{t} {', '.join(n)};" for t, n in fields
    )

    yield f"typedef struct {name} {{ {fields} }} {name};"



def vec_def(v : VecDefArgs):
    yield from vec_def_struct(v)
    yield from vec_def_basic(v)

    if v.fields == VEC3:
        yield from vec_def_xyz(v)

    yield from vec_def_array_conversions(v)
    yield from vec_def_accessors(v)

    if v.is_integer:
        yield from vec_def_integer_ops(v)

def vec_def_struct(v : VecDefArgs):
    yield from def_global_struct(v.vtype, [(v.ctype, v.fields)])

def vec_def_basic(v : VecDefArgs):
    yield from vec_def_binary(v, "Add", op2("+"))
    yield from vec_def_binary(v, "Sub", op2("-"))
    yield from vec_def_unary(v, "Neg", op1("-"))
    yield from vec_def_reduced_binary(v, "Dot", v.ctype, nary_reduce(op2("+")), op2("*"))
    yield from vec_def_self_adjoint(v, "LengthSq", v.ctype, nary_reduce(op2("+")), op2("*"))
    yield from def_func(v, "DistanceSq", [(v.vtype, "a"), (v.vtype, "b")], v.ctype,
        call(v, "LengthSq", [call(v, "Sub", ["a", "b"])])
    )
    yield from vec_def_scalar(v, "Mul", op2("*"))
    yield from vec_def_rscalar(v, "RMul", op2("*"))
    yield from vec_def_rscalar(v, "Div", op2("/"))
    yield from vec_def_binary(v, "HdMul", op2("*"))
    yield from vec_def_binary(v, "HdDiv", op2("/"))
    yield from vec_def_binary(v, "HdMin", fun2("LAB_MIN"))
    yield from vec_def_binary(v, "HdMax", fun2("LAB_MAX"))
    yield from vec_def_reduced(v, "GetMin", v.ctype, MIN_NARY)
    yield from vec_def_reduced(v, "GetMax", v.ctype, MAX_NARY)
    yield from vec_def_reduced(v, "MinIndex", "size_t", MIN_INDEX_NARY)
    yield from vec_def_reduced(v, "MaxIndex", "size_t", MAX_INDEX_NARY)

    yield from vec_def_unary(v, "Abs", fun1(ABS_OVERLOADS[v.ctype]))

def vec_def_binary(v : VecDefArgs, name : str, op : BinaryTemplate):
    expr = nary_new(v.vtype)(*map_components(op, v.fields, ["a", "b"]))
    yield from def_func(v, name, [(v.vtype, "a"), (v.vtype, "b")], v.vtype, expr)

def vec_def_unary(v : VecDefArgs, name : str, op : UnaryTemplate):
    expr = nary_new(v.vtype)(*map_components(op, v.fields, ["a"]))
    yield from def_func(v, name, [(v.vtype, "a")], v.vtype, expr)

def vec_def_scalar(v : VecDefArgs, name : str, op : BinaryTemplate):
    expr = nary_new(v.vtype)(*map_components(lambda b: op("a", b), v.fields, ["b"]))
    yield from def_func(v, name, [(v.ctype, "a"), (v.vtype, "b")], v.vtype, expr)

def vec_def_rscalar(v : VecDefArgs, name : str, op : BinaryTemplate):
    expr = nary_new(v.vtype)(*map_components(lambda a: op(a, "b"), v.fields, ["a"]))
    yield from def_func(v, name, [(v.vtype, "a"), (v.ctype, "b")], v.vtype, expr)

def vec_def_reduced_binary(v : VecDefArgs, name : str, ret : ComplexType, reduceop : NaryTemplate, compop : BinaryTemplate):
    expr = reduceop(*map_components(compop, v.fields, ["a", "b"]))
    yield from def_func(v, name, [(v.vtype, "a"), (v.vtype, "b")], ret, expr)

def vec_def_reduced_unary(v : VecDefArgs, name : str, ret : ComplexType, reduceop : NaryTemplate, compop : UnaryTemplate):
    expr = reduceop(*map_components(compop, v.fields, ["a"]))
    yield from def_func(v, name, [(v.vtype, "a")], ret, expr)

def vec_def_self_adjoint(v : VecDefArgs, name : str, ret : ComplexType, reduceop : NaryTemplate, compop : BinaryTemplate):
    yield from vec_def_reduced_unary(v, name, ret, reduceop, self_adjoint(compop))

def vec_def_reduced(v : VecDefArgs, name : str, ret : ComplexType, reduceop : NaryTemplate):
    expr = reduceop(*components(v.fields, "a"))
    yield from def_func(v, name, [(v.vtype, "a")], ret, expr)






## 3D-Vector: with xyz
def vec_def_xyz(v : VecDefArgs):
    def xyz_create(name, func_prefix):
        x_fn = func_prefix.replace("#", "X")
        y_fn = func_prefix.replace("#", "Y")
        z_fn = func_prefix.replace("#", "Z")
        expr = f"({v.vtype}) {{ {x_fn}(i), {y_fn}(i), {z_fn}(i) }}"
        yield from def_func(v, name, [("LAB_DirIndex", "i")], v.vtype, expr)

    def xyz_r_op(name, op, func_prefix):
        x_fn = func_prefix.replace("#", "X")
        y_fn = func_prefix.replace("#", "Y")
        z_fn = func_prefix.replace("#", "Z")
        x = op("a.x", f"{x_fn}(i)")
        y = op("a.y", f"{y_fn}(i)")
        z = op("a.z", f"{z_fn}(i)")
        expr = f"({v.vtype}) {{ {x}, {y}, {z} }}"
        yield from def_func(v, name, [(v.vtype, "a"), ("LAB_DirIndex", "i")], v.vtype, expr)

    yield from xyz_create("FromDirIndex", "LAB_O#")
    yield from xyz_create("Dir", "LAB_O#")
    yield from xyz_create("Dir_OrthA", "LAB_A#F")
    yield from xyz_create("Dir_OrthB", "LAB_B#F")

    yield from xyz_r_op("AddDir", op2("+"), "LAB_O#")
    yield from xyz_r_op("SubDir", op2("-"), "LAB_O#")






def vec_def_array_conversions(v : VecDefArgs):
    expr = nary_new(v.vtype)(*(f"array[{i}]" for i in range(len(v.fields))))
    yield from def_func(v, "FromArray", [(cptr(v.ctype), "array")], v.vtype, expr, attrs=LAB_VEC_DECL_PURE)

    expr = f"&a->{v.fields[0]}"
    yield from def_func(v, "AsArray", [(ptr(v.vtype), "a")], ptr(v.ctype), expr, attrs=LAB_VEC_DECL)
    yield from def_func(v, "AsCArray", [(cptr(v.vtype), "a")], cptr(v.ctype), expr, attrs=LAB_VEC_DECL)

    stmt_assign = " ".join(f"array[{i}] = a.{f};" for i, f in enumerate(v.fields))
    stmt_return = "return array;"
    yield from def_proc(v, "ToArray", [(ptr(v.ctype), "array"), (v.vtype, "a")], ptr(v.ctype), f"{stmt_assign} {stmt_return}")

    stmt_assign = " ".join(f"*p{f} = a.{f};" for f in v.fields)
    ptr_args = [(ptr(v.ctype), f"p{f}") for f in v.fields]
    yield from def_proc(v, "Unpack", [*ptr_args, (v.vtype, "a")], "void", stmt_assign)

def vec_def_accessors(v : VecDefArgs):
    expr = f"{v.vtype}_AsArray(&a)[i]"
    yield from def_func(v, "Get", [(v.vtype, "a"), ("size_t", "i")], v.ctype, expr)

    expr = f"&{v.vtype}_AsArray(a)[i]"
    yield from def_func(v, "Ref", [(ptr(v.vtype), "a"), ("size_t", "i")], ptr(v.ctype), expr)

    expr = f"&{v.vtype}_AsCArray(a)[i]"
    yield from def_func(v, "CRef", [(cptr(v.vtype), "a"), ("size_t", "i")], cptr(v.ctype), expr)



def vec_def_integer_ops(v : VecDefArgs):
    yield from vec_def_rscalar(v, "Sar", fun2("LAB_Sar"))
    yield from vec_def_rscalar(v, "Shr", op2(">>"))
    yield from vec_def_rscalar(v, "Shl", op2("<<"))

    yield from vec_def_reduced_binary(v, "Equals", "bool", nary_reduce(op2("&&")), op2("=="))













@dataclass
class BoxDefArgs(DefArgs):
    btype : TypeTok
    vec : VecDefArgs

    @property
    def vtype(self): return self.vec.vtype
    @property
    def ctype(self): return self.vec.ctype
    @property
    def vec_fields(self): return self.vec.fields
    @property
    def is_integer(self): return self.vec.is_integer
    @property
    def is_float(self): return self.vec.is_float

    @property
    def namespace(self): return self.btype

def box_def(b : BoxDefArgs):
    yield from def_global_struct(b.btype, [(b.vtype, ["a", "b"])])

    yield from box_def_box_vec(b, "Add", fun2(b.vec.name("Add")))
    #yield from box_def_binary(b, "Expand", fun2(b.vec.name("Add"))) # TODO: expand by another box

    expr = f"({b.btype}) {{ {b.vec.name('Sub')}(box.a, vec), {b.vec.name('Add')}(box.b, vec) }}"
    yield from def_func(b, "Expand", [(b.btype, "box"), (b.vtype, "vec")], b.btype, expr)


    expr = f"({b.btype}) {{ {b.vec.name('Mul')}(factor, box.a), {b.vec.name('Mul')}(factor, box.b) }}"
    yield from def_func(b, "Mul", [(b.ctype, "factor"), (b.btype, "box")], b.btype, expr)

    expr = " && ".join(f"box.a.{f} <= v.{f} && v.{f} <= box.b.{f}" for f in b.vec_fields)
    yield from def_func(b, "Contains_Inc", [(b.btype, "box"), (b.vtype, "v")], "bool", expr)

    expr = " || ".join(f"box.a.{f} >= box.b.{f}" for f in b.vec_fields)
    yield from def_func(b, "IsEmpty", [(b.btype, "box")], "bool", expr)

    yield from box_def_binary(b, "Intersection", fun2(b.vec.name("HdMax")), fun2(b.vec.name("HdMin")))

    expr = f"!{b.name('IsEmpty')}({b.name('Intersection')}(a, b))"
    yield from def_func(b, "Intersects", [(b.btype, "a"), (b.btype, "b")], "bool", expr)

    expr = f"{b.vec.name('Sub')}(box.b, box.a)"
    yield from def_func(b, "Size", [(b.btype, "box")], b.vtype, expr)

    if b.is_float:
        coords = ", ".join(f"box.a.{f} + (box.b.{f} - box.a.{f}) * v.{f}" for f in b.vec_fields)
        expr = f"({b.vtype}) {{ {coords} }}"
        yield from def_func(b, "MapPoint", [(b.btype, "box"), (b.vtype, "v")], b.vtype, expr)

        expr = f"({b.btype}) {{ {b.name('MapPoint')}(box, fraction.a), {b.name('MapPoint')}(box, fraction.b) }}"
        yield from def_func(b, "Map", [(b.btype, "box"), (b.btype, "fraction")], b.btype, expr)


    expr = f"({b.btype}) {{ origin, {b.vec.name('Add')}(origin, size) }}"
    yield from def_func(b, "FromOriginAndSize", [(b.vtype, "origin"), (b.vtype, "size")], b.btype, expr)


    yield from box_def_array_conversions(b)

    for f in b.vec_fields:
        expr = f"a.b.{f} - a.a.{f}"
        yield from def_func(b, f"D{f.capitalize()}", [(b.btype, "a")], b.ctype, expr)



def box_def_box_vec(b : BoxDefArgs, name : str, op_a : BinaryTemplate, op_b : BinaryTemplate = None):
    if op_b == None: op_b = op_a
    expr = nary_new(b.btype)(op_a("a.a", "b"), op_b("a.b", "b"))
    yield from def_func(b, name, [(b.btype, "a"), (b.vtype, "b")], b.btype, expr)

def box_def_binary(b : BoxDefArgs, name : str, op_a : BinaryTemplate, op_b : BinaryTemplate = None):
    if op_b == None: op_b = op_a
    expr = nary_new(b.btype)(
        *map_components(op_a, ["a"], ["a", "b"]),
        *map_components(op_b, ["b"], ["a", "b"])
    )
    yield from def_func(b, name, [(b.btype, "a"), (b.btype, "b")], b.btype, expr)


def box_def_array_conversions(b : BoxDefArgs):
    array_type = ptr(array(b.ctype, len(b.vec_fields)))
    carray_type = ptr(carray(b.ctype, len(b.vec_fields)))
    expr = cast1(array_type)(f"&a->a.{b.vec_fields[0]}")
    cexpr = cast1(carray_type)(f"&a->a.{b.vec_fields[0]}")
    yield from def_func(b, "AsMDArray", [(ptr(b.btype), "a")], array_type, expr, attrs=LAB_VEC_DECL)
    yield from def_func(b, "AsCMDArray", [(cptr(b.btype), "a")], carray_type, cexpr, attrs=LAB_VEC_DECL)

    expr = f"&a->a"
    yield from def_func(b, "AsArray", [(ptr(b.btype), "a")], ptr(b.vtype), expr, attrs=LAB_VEC_DECL)
    yield from def_func(b, "AsCArray", [(cptr(b.btype), "a")], cptr(b.vtype), expr, attrs=LAB_VEC_DECL)

    stmt_assign = " ".join(f"*p{i}{f} = a.{i}.{f};" for i, f in product("ab", b.vec_fields))
    ptr_args = [(ptr(b.ctype), f"p{i}{f}") for i, f in product("ab", b.vec_fields)]
    yield from def_proc(b, "Unpack", [*ptr_args, (b.btype, "a")], "void", stmt_assign)

    stmt_assign_a = " ".join(f"*p{f} = a.a.{f};" for f in b.vec_fields)
    stmt_assign_b = " ".join(f"*pd{f} = a.b.{f}-a.a.{f};" for f in b.vec_fields)
    stmt_assign = f"{stmt_assign_a} {stmt_assign_b}"
    ptr_args = [(ptr(b.ctype), f"p{i}{f}") for i, f in product(["", "d"], b.vec_fields)]
    yield from def_proc(b, "Unpack_Sized", [*ptr_args, (b.btype, "a")], "void", stmt_assign)

    a_fields = ", ".join(f"{f}" for f in b.vec_fields)
    b_fields = ", ".join(f"{f} + d{f}" for f in b.vec_fields)
    expr = f"({b.btype}) {{ {{ {a_fields} }}, {{ {b_fields} }} }}"
    args = [(b.ctype, f"{i}{f}") for i, f in product(["", "d"], b.vec_fields)]
    yield from def_func(b, "New_Sized", args, b.btype, expr)



def def_all(vec_fields : List[NameTok]):
    N = len(vec_fields)


    def conv_func_name(prefix, src, dst, suffix):
        if suffix is None:
            return f"{prefix}{N}{src}2{dst}"
        else:
            return f"{prefix}{N}{src}2{dst}_{suffix}"


    vec_args = {}
    for t, ctype, is_integer in TYPES:
        vtype = f"LAB_Vec{N}{t}"
        vec_args[t] = VecDefArgs(vtype=vtype, ctype=ctype, fields=vec_fields, is_integer=is_integer)

    for t in TYPE_SUFFIXES:
        yield from vec_def(vec_args[t])
        yield ""

    def vec_def_conv(name, src : VecDefArgs, dst : VecDefArgs, comp_conv : UnaryTemplate):
        expr = nary_new(dst.vtype)(*map_components(comp_conv, src.fields, ["a"]))
        yield from def_global_func(name, [(src.vtype, "a")], dst.vtype, expr)

    for src, dst, suffix, fun in CONVERSIONS:
        name = conv_func_name("LAB_Vec", src, dst, suffix)
        asrc = vec_args[src]
        adst = vec_args[dst]
        if fun is None: fun = cast1(adst.ctype)
        yield from vec_def_conv(name, asrc, adst, fun)

    yield ""

    box_args = {}
    for t in TYPE_SUFFIXES:
        box_args[t] = BoxDefArgs(f"LAB_Box{N}{t}", vec_args[t])

    for t in TYPE_SUFFIXES:
        yield from box_def(box_args[t])
        yield ""


    for src, dst, suffix, fun in CONVERSIONS:
        name = conv_func_name("LAB_Box", src, dst, suffix)
        vname = conv_func_name("LAB_Vec", src, dst, suffix)
        asrc = box_args[src]
        adst = box_args[dst]
        if fun is None: fun = cast1(adst.ctype)#

        expr = f"({adst.btype}) {{ {vname}(a.a), {vname}(a.b) }}"
        yield from def_global_func(name, [(asrc.btype, "a")], adst.btype, expr)





def print_all(gen, **printkw):
    for l in gen:
        print(l, **printkw)


def abs_filename(filename : str):
    return os.path.join(sp(sp(os.path.abspath(__file__))), "include", filename)


if __name__=="__main__":
    sp = lambda p: os.path.split(p)[0]

    for fn, vecspec, header in FILES:
        with open(abs_filename(fn), "w") as f:
            printkw = {"file": f}

            print(HEADER.strip(), **printkw)
            print(**printkw)
            print(header.strip(), **printkw)
            print(**printkw)
            print(**printkw)
            print_all(def_all(vecspec), **printkw)

    filename = abs_filename("LAB_vec.h")

    #print(filename)
    with open(filename, "w") as f:
        printkw = {"file": f}

        print("#pragma once", **printkw)
        print(**printkw)
        for fn, vecspec, header in FILES:
            print(f"#include \"{fn}\"", **printkw)
        print(**printkw)
        for fn, vecspec, header in FILES:
            print(f"// export from {fn}", **printkw)
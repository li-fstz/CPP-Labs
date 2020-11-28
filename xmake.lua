set_symbols("debug")

add_cflags("-fexec-charset=GBK", {force = true})

local libs = {
    "rule", 
    "voidtable", 
    "first",
    "follow", 
    "parsingtable",
    "pickupleftfactor",
    "removeleftrecursion1", 
    "removeleftrecursion2", 
    "parser"
}

for _, lib in pairs(libs) do 
    target(lib)
        set_kind("static")
        add_files("src/inc/" .. lib .. ".c")
end

local labs = {
    lab1 = {"rule", "voidtable"},
    lab2 = {"rule", "voidtable", "first"},
    lab3 = {"rule", "voidtable", "first", "follow"},
    lab4 = {"rule", "voidtable", "first", "follow", "parsingtable"},
    lab5 = {"rule", "pickupleftfactor"},
    lab6 = {"rule", "removeleftrecursion1"},
    lab7 = {"rule", "removeleftrecursion2"},
    lab8 = {"rule", "voidtable", "first", "follow", "parsingtable", "parser"}
}

for lab, include in pairs(labs) do
    target(lab)
        set_kind("binary")
        set_targetdir("build")
        add_includedirs("src/inc")
        add_files("src/" .. lab .. ".c")
        for _, lib in pairs(include) do
            add_deps(lib)
        end
end

-- xmake f  --toolchain=gcc --cc=D:\MinGW\bin\gcc.exe

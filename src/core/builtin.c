"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"module builtin;\n"
"export import \"builtin_c\";\n"
"\n"
"\n"
"import \"system\";\n"
"import \"io\";\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"static _eel_version = [__version(0), __version(1), __version(2)];\n"
"if __version(3)\n"
"\t_eel_version[3] = __version(3);\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"static _lib_version = [0, 3, 7];\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"export function eel_version\n"
"{\n"
"\treturn _eel_version;\n"
"}\n"
"\n"
"export function builtin_version\n"
"{\n"
"\treturn _lib_version;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"export function compile(s)[flags, filename, modname]\n"
"{\n"
"\tlocal m = module [];\n"
"\tif specified flags\n"
"\t\tlocal f = flags;\n"
"\telse\n"
"\t\tf = 0;\n"
"\tm.__source = s;\n"
"\tif specified filename\n"
"\t\tlocal fn = filename;\n"
"\telse\n"
"\t\tfn = nil;\n"
"\tif specified modname\n"
"\t\tlocal mn = modname;\n"
"\telse\n"
"\t\tmn = nil;\n"
"\tif fn\n"
"\t\tm.__filename = fn;\n"
"\telse\n"
"\t\tm.__filename = \"script from \" + ((string)typeof s) + \", \" +\n"
"\t\t\t\t(string)sizeof s + \" bytes\";\n"
"\tif mn\n"
"\t\tm.__modname = mn;\n"
"\telse\n"
"\t\tm.__modname = m.__filename;\n"
"\tif not (f & SF_NOCOMPILE)\n"
"\t{\n"
"\t\t__compile(m, f);\n"
"\t\tif not (f & SF_NOINIT)\n"
"\t\t\tm.__init_module();\n"
"\t}\n"
"\treturn m;\n"
"}\n"
"\n"
"\n"
"\n"
"export function __load_eel_module(fullpath, flags)\n"
"{\n"
"\tlocal f = file [fullpath, \"rb\"];\n"
"\tlocal buf = read(f, sizeof f);\n"
"\treturn compile(buf, flags, fullpath);\n"
"}\n"
"\n"
"$.__load_eel_module = __load_eel_module;\n"
"\n"
"\n"
"\n"
"export function __load_via_path_modules(modname, flags)\n"
"{\n"
"\n"
"\n"
"\tfunction try_load(p, f)\n"
"\t{\n"
"\t\tlocal ldeel = $.__load_eel_module;\n"
"\t\ttry\n"
"\t\t\tif copy(p, sizeof p - 4) != \".eel\"\n"
"\t\t\t\treturn ldeel(p + \".eel\", f);\n"
"\t\ttry\n"
"\t\t\tif copy(p, sizeof p - 4) != \".ess\"\n"
"\t\t\t\treturn ldeel(p + \".ess\", f);\n"
"\t\ttry\n"
"\t\t\treturn ldeel(p, f);\n"
"\t\ttry\n"
"\t\t\tif copy(p, sizeof p - sizeof SOEXT) != SOEXT\n"
"\t\t\t\treturn __load_binary(p + SOEXT, f);\n"
"\t\ttry\n"
"\t\t\treturn __load_binary(p, f);\n"
"\t\treturn nil;\n"
"\t}\n"
"\tlocal modpaths = $.path_modules;\n"
"\tfor local i = 0, sizeof modpaths - 1\n"
"\t{\n"
"\t\tif sizeof modpaths[i]\n"
"\t\t\tlocal m = try_load(modpaths[i] + DIRSEP + modname, flags);\n"
"\t\telse\n"
"\t\t\tm = try_load(modname, flags);\n"
"\t\tif m\n"
"\t\t\treturn m;\n"
"\t}\n"
"\tthrow __exports().XMODULELOAD;\n"
"}\n"
"\n"
"\n"
"export function __load_injected_module(modname, flags)\n"
"{\n"
"\treturn $.injected_modules[modname](flags);\n"
"}\n"
"\n"
"\n"
"\n"
"$.module_loaders = [\n"
"\t__get_loaded_module,\t\n"
"\t__load_injected_module,\t\n"
"\t__load_via_path_modules\t\n"
"];\n"
"\n"
"\n"
"\n"
"export function load(modname)[flags]\n"
"{\n"
"\tlocal x = $.module_loaders;\n"
"\tif specified flags\n"
"\t\tlocal f = flags;\n"
"\telse\n"
"\t\tf = 0;\n"
"\tfor local i = 0, sizeof x - 1\n"
"\t{\n"
"\t\tlocal load_error = nil;\n"
"\t\ttry\n"
"\t\t{\n"
"\t\t\tlocal m = x[i](modname, f);\n"
"\t\t\tif typeof m == module\n"
"\t\t\t\treturn m;\n"
"\t\t\tload_error = \"load(): \" + x[i].name + \"(\\\"\" +\n"
"\t\t\t\t\t(string)modname + \"\\\"\" + \", \" +\n"
"\t\t\t\t\t(string)f + \") returned \" + (string)m +\n"
"\t\t\t\t\t\" instead of a module\";\n"
"\t\t}\n"
"\t\tif load_error\n"
"\t\t\tthrow load_error;\n"
"\t}\n"
"\tthrow \"load(): Could not load module \\\"\" + (string)modname + \"\\\"\";\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"export function exception_info(x)\n"
"{\n"
"\tlocal r = table [];\n"
"\tr.code = x;\n"
"\ttry\n"
"\t\tr.name = exception_name(x);\n"
"\texcept\n"
"\t\tr.name = \"\";\n"
"\ttry\n"
"\t\tr.description = exception_description(x);\n"
"\texcept\n"
"\t\tr.description = \"(No description available.)\";\n"
"\treturn r;\n"
"}\n"
"\n"
"try\n"
"\tfor local x = 0, 99999\n"
"\t\t__exports()[exception_name((integer)x)] = (integer)x;\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"export function deepclone(src)[_level]\n"
"{\n"
"\tif specified _level\n"
"\t\tlocal level = _level + 1;\n"
"\telse\n"
"\t\tlevel = 0;\n"
"\tif level > 1000\n"
"\t\tthrow \"deepclone(): Infinite recursion aborted!\";\n"
"\tswitch typeof src\n"
"\t  case array\n"
"\t  {\n"
"\t\tlocal a = [];\n"
"\t\tfor local i = 0, sizeof src - 1\n"
"\t\t\ta[i] = deepclone(src[i], level);\n"
"\t\treturn a;\n"
"\t  }\n"
"\t  case table\n"
"\t  {\n"
"\t\tlocal t = table [];\n"
"\t\tfor local i = 0, sizeof src - 1\n"
"\t\t\tt[key(src, i)] = deepclone(index(src, i), level);\n"
"\t\treturn t;\n"
"\t  }\n"
"\t  default\n"
"\t\treturn clone src;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"export function deepcompare(l, r)[_level]\n"
"{\n"
"\tif specified _level\n"
"\t\tlocal level = _level + 1;\n"
"\telse\n"
"\t\tlevel = 0;\n"
"\tif level > 1000\n"
"\t\tthrow \"deepcompare(): Infinite recursion aborted!\";\n"
"\tswitch typeof l\n"
"\t  case array\n"
"\t  {\n"
"\t\tif typeof r != array\n"
"\t\t\treturn false;\n"
"\t\tif sizeof l != sizeof r\n"
"\t\t\treturn false;\n"
"\t\tfor local i = 0, sizeof l - 1\n"
"\t\t\tif not deepcompare(l[i], r[i], level)\n"
"\t\t\t\treturn false;\n"
"\t\treturn true;\n"
"\t  }\n"
"\t  case table\n"
"\t  {\n"
"\t\tif typeof r != table\n"
"\t\t\treturn false;\n"
"\t\tif sizeof l != sizeof r\n"
"\t\t\treturn false;\n"
"\t\tfor local i = 0, sizeof l - 1\n"
"\t\t{\n"
"\t\t\tlocal k = key(l, i);\n"
"\t\t\tif not deepcompare(l[k], r[k], level)\n"
"\t\t\t\treturn false;\n"
"\t\t}\n"
"\t\treturn true;\n"
"\t  }\n"
"\t  default\n"
"\t\treturn l == r;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"$.cleanup = [];\n"
"\n"
"export procedure __cleanup\n"
"{\n"
"\tfor local i = 0, sizeof $.cleanup - 1\n"
"\t\t$.cleanup[i]();\n"
"\tdelete($);\n"
"}\n"

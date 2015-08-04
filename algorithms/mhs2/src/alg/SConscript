from os.path import join
Import('env')


def find_sources(root, depth=4):
    sources = []
    wildcards = root

    for i in range(depth):
        wildcards = join(wildcards, "*")
        sources += env.Glob(wildcards + "cpp")
    return sources


common = find_sources("common")

env.Append(CPPPATH = ["common"],
           LINKFLAGS = " -lboost_system -lpthread")
obj = env.Program(target = "mhs2", source = find_sources("app") + common)

if env['debug']:
    print "Compiling tests"
    obj = env.Program(target = "test", source = find_sources("tests") + common)

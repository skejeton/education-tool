import os
import sys

class Arguments:
  def __init__(self, target, switches):
    self.target = target
    self.switches = switches

def parse_arguments(argv):
  target = ""
  switches = []

  for arg in argv:
    if arg.startswith("-"):
      switches.append(arg[1:])  
    else:
      if target == "":
        target = arg
      else:
        raise Exception(f"Conflicting targets: {target} and {arg}")

  if target == "":
    raise Exception("No target specified")

  return Arguments(target, switches)

def target_build():
  if os.name == "nt":
    return "if not exist bin mkdir bin\nlib\\sokol-tools-bin\\bin\\win32\\sokol-shdc.exe --input src/shaders/amalgamation.glsl --output src/shaders.hxx --slang hlsl5:glsl100 && cd bin && cmake -DCMAKE_BUILD_TYPE=Debug .. && msbuild catedu.sln /property:Configuration=Debug && cd .."
  else:
    return "mkdir -p bin\nlib/sokol-tools-bin/bin/linux/sokol-shdc --input src/shaders/amalgamation.glsl --output src/shaders.hxx --slang glsl330 && cd bin && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && cd .."

def target_run():
  if os.name == "nt":
    return target_build() + " && .\\bin\\debug\\catedu.exe\n"
  else:
    return target_build() + " && ./bin/debug/catedu.exe\n"

def main():
  arguments = parse_arguments(sys.argv[1:])
  script = ""

  if arguments.target == "build":
    script = target_build()
  elif arguments.target == "run":
    script = target_run()
  else:
    raise Exception(f"Unknown target: {arguments.target}")

  print(script)
  lines = script.split('\n')
  for line in lines:
    os.system(line)


if __name__ == "__main__":
  main()

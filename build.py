import os
import sys
import shutil

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

system_features = {
  "has_emcc": False
}

def target_build_wasm():
  if os.name != "nt" and system_features["has_emcc"]:
    return "./scripts/build-wasm.sh"
  else:
    if os.name == "nt":
      raise Exception("Can not build WASM on Windows, please, install WSL2 with Emscripten SDK")
    if not system_features["has_emcc"]:
      raise Exception("Can not build WASM because you need to install Emscripten SDK")

def target_build(test_mode = False, env_test = False, env_test_name = ""):
  test_string = ""
  if test_mode and env_test:
    test_string += f"-DENV_TYPE=ENVTEST -DENV_TEST_NAME={env_test_name}"
  elif test_mode:
    test_string += "-DENV_TYPE=UNITTEST"
  else:
    test_string += "-DENV_TYPE=NORMAL"

  if os.name == "nt":
    return f"if not exist bin mkdir bin\nlib\\sokol-tools-bin\\bin\\win32\\sokol-shdc.exe --input catedu/shaders/amalgamation.glsl --output catedu/shaders.hxx --slang hlsl5 && cd bin && cmake -DCMAKE_BUILD_TYPE=Debug {test_string} .. && msbuild catedu.sln /property:Configuration=Debug && cd .."
  elif sys.platform == "darwin":
    return f"mkdir -p bin\nlib/sokol-tools-bin/bin/osx/sokol-shdc --input catedu/shaders/amalgamation.glsl --output catedu/shaders.hxx --slang metal_macos && cd bin && cmake -DCMAKE_BUILD_TYPE=Debug {test_string} .. && make && cd .."
  else:
    return f"mkdir -p bin\nlib/sokol-tools-bin/bin/linux/sokol-shdc --input catedu/shaders/amalgamation.glsl --output catedu/shaders.hxx --slang glsl330 && cd bin && cmake -DCMAKE_BUILD_TYPE=Debug {test_string} .. && make && cd .."

def target_run(test_mode = False, env_test = False, env_test_name = ""):
  if os.name == "nt":
    return target_build(test_mode, env_test, env_test_name) + " && .\\bin\\debug\\catedu.exe\n"
  elif sys.platform == "darwin":
    return target_build(test_mode, env_test, env_test_name) + " && ./bin/catedu\n"
  else:
    return target_build(test_mode, env_test, env_test_name) + " && ./bin/debug/catedu\n"

def target_test_unit():
  return target_run(True)

def target_test_env(test: str):
  return target_run(True, True, test)

def init_system_features():
  if shutil.which("emcc") is not None:
    system_features["has_emcc"] = True

def main():
  arguments = parse_arguments(sys.argv[1:])
  script = ""

  init_system_features()

  if arguments.target == "build-wasm":
    script = target_build_wasm()
  elif arguments.target == "build":
    script = target_build()
  elif arguments.target == "test-unit":
    script = target_test_unit()
  elif arguments.target == "test-env":
    assert(len(arguments.switches) == 1)
    script = target_test_env(arguments.switches[0])
  elif arguments.target == "run":
    script = target_run()
  else:
    raise Exception(f"Unknown target: {arguments.target}")
  print(script)
  if os.name == "nt":
    os.system("if not exist user mkdir user")
  else:
    os.system("mkdir -p user")
  lines = script.split('\n')
  for line in lines:
    status = os.system(line)
    if status != 0:
      exit(status)


if __name__ == "__main__":
  main()

#!/usr/bin/env python3

import subprocess


def log(msg):
    print(f'\033[34mLOG: {msg}\033[0m')


def shell_exec(cmd, timeout_s=5, check_succ=False):
    cmd_copy     = []
    quoter_parts = []
    quoter_type  = '"'
    for p in cmd.split():
        if quoter_parts:
            if not p.endswith(quoter_type):
                quoter_parts.append(p)
            else:
                quoter_parts.append(p[:-1])
                cmd_copy.append(' '.join(quoter_parts))
                quoter_parts.clear()
        else:
            if p.startswith('"'):
                quoter_type = '"'
                if p.endswith(quoter_type):
                    cmd_copy.append(p)
                else:
                    quoter_parts.append(p[1:])
            elif p.startswith("'"):
                quoter_type = "'"
                if p.endswith(quoter_type):
                    cmd_copy.append(p)
                else:
                    quoter_parts.append(p[1:])
            else:
                cmd_copy.append(p)
    log('C>[{}]'.format(' '.join(cmd_copy)))
    res = subprocess.run(cmd_copy, capture_output=True, timeout=timeout_s)
    ec  = res.returncode
    out = res.stdout.decode('utf-8').rstrip()
    if out and len(out.split('\n')) > 1:
        log(f'R({ec})<\n{out}')
    else:
        log(f'R({ec})<[{out}]')
    if check_succ:
        res.check_returncode()
    return ec, out


def gen_warning():
    return """# This is autogen file by tools/travis_yml_generator.py
# Do not change it manualy because after a next `git push` command this file
#   will be regenerated.
#

"""


def last_green_travis_build():
    last_green = ''
    if not last_green:
        _, init_commit = shell_exec('git rev-list --max-parents=0 HEAD', check_succ=True)
        last_green = init_commit.strip()
    return last_green


def gen_travis_yaml(changed):
    if not changed: return False
    stage_parts = []
    for changed_dir in changed:
        yml_part = '    - script: '
        if not stage_parts:
            yml_part = '    - stage: Build && Test && Package\n      script: '
        commands = [
            f"echo '{changed_dir}'",
            f"pushd '{changed_dir}'",
            "mkdir build",
            "cd build",
            "cmake ..",
            "cmake --build .",
            "cmake --build . --target test",
            "cmake --build . --target package",
            "popd",
        ]
        yml_part += ' && '.join(commands)
        stage_parts.append(yml_part)

    TRAVIS_YAML_NAME = '.travis.yml'
    with open(TRAVIS_YAML_NAME, 'w') as f:
        f.write(gen_warning())
        f.write("""language: cpp

before_script:
- sudo apt-get install libboost-test-dev -y
- echo "deb http://archive.ubuntu.com/ubuntu xenial main universe" | sudo tee -a /etc/apt/sources.list
- sudo apt-get update -qq

jobs:
  include:
""")
        for part in stage_parts: f.write(part)
#        f.write("""
#deploy:
#  provider: script
#  skip_cleanup: true
#  script:
#  - curl -T helloworld-0.0.$TRAVIS_BUILD_NUMBER-Linux.deb -ustarokurov:$BINTRAY_API_KEY "https://api.bintray.com/content/starokurov/otus-cpp/helloworld/$TRAVIS_BUILD_NUMBER/helloworld-0.0.$TRAVIS_BUILD_NUMBER-Linux.deb;deb_distribution=trusty;deb_component=main;deb_architecture=amd64;publish=1"
#""")
    return TRAVIS_YAML_NAME


def commit_changes(yml):
    shell_exec(f'git add {yml}', check_succ=True)
    _, out = shell_exec('git status -s', check_succ=True)
    if [1 for line in out.split('\n') if not line.startswith('??')]:
        shell_exec(f'git commit -m "[AUTO] regen {yml} before pushing"')


def main():
    DIR_PREFIX = 'hw'
    last_green = last_green_travis_build()
    _, out = shell_exec(f'git diff --name-only HEAD {last_green}', check_succ=True)
    changed = {line.split('/')[0] for line in out.split('\n') if line.startswith(DIR_PREFIX)}
    if changed:
        yml_name = gen_travis_yaml(changed)
        commit_changes(yml_name)


if __name__ == '__main__':
    main()


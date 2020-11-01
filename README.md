# otus-cpp [![Build Status](https://travis-ci.com/kadze009/otus-cpp.svg?branch=main)](https://travis-ci.com/kadze009/otus-cpp)

This is the repository with homeworks of [otus c++'s](https://otus.ru/learning/73720/) courses.

## Useful links
* [Travic CI](https://travis-ci.com/github/kadze009/otus-cpp)
* [Bintray](https://bintray.com/kadze009/otus-cpp)

## CI details
There are some [scripts](scripts) which make CI usage more strict and
predictable. The main idea is to correctly tell to Travis CI to work only with
projects (read as directories) which was changed. For this goal should be used
*pre-push* git's hook and a custom script which will generate *.travis.yml*
configuration each pushing time. Additionally the script should know the last
*green* commit (changes should pass tests in Travis CI). All of these tasks
does [scripts/travis\_yml\_generator.py](scripts/travis_yml_generator.py).


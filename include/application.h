/*
 *
 *
 * ⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠛⠛⠛⠋⢉⣉⣀⡤
⠀*  ⠰⡄⠹⣿⣿⣿⣿⠿⠛⠛⠛⠉⠉⠉⠀⠀⢠⣤⣤⣴⣶⣶⣿⣿⣿⣿⡿⠁
⠀*  ⠀⠘⣦⠈⢿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠛⠿⣿⣿⣿⣿⡟⠀
⠀*  ⠀⠀⠘⢷⡄⠹⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⡟⠀
⠀*  ⠀⠀⠀⠈⢿⣦⠈⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⠏⠀
⠀*  ⠀⠀⠀⠀⠈⢻⣷⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⠏⠀
⠀*  ⠀⠀⠀⠀⠀⠀⢻⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⠃
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠻⣿⣷⡄⠀⠀⠀⢀⣠⡞⠀⣾⡿⠃
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣦⣠⣴⣿⣿⠁⣸⡿⠁
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⠇⢰⡟⠁
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⡟⢀⡟
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡿⠁⠎
⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠃
 *
 *    Cry - Cryptor
 *    An ignifi Project © 2023
 *
 *    Licence : BSD 3-Clause License
 *    see file: LICENCE
 *
 *    Contact: cry.2023.gitlab.x9j3f22@ignifi.eu
 *
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include <cry/cry.h>

#include <memory>

namespace cry {

class Application {
public:
  Application(int argc, char **argv) : _argc(argc), _argv(argv) {}

  ~Application() = default;

  // ========================================

  void ParseOptions();

  void DisplayVersion() const;

private:
  int _argc;
  char **_argv;
  std::unique_ptr<Cry> _cry = std::make_unique<Cry>();
};

} // namespace cry

#endif // APPLICATION_H

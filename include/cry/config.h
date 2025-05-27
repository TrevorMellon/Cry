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

#ifndef CRY_CONFIG
#define CRY_CONFIG

#pragma once

#include <gcrypt.h>
#include <stdint.h>

constexpr const size_t cry_buffer_multiplier    = 16;

constexpr const gcry_cipher_algos gpg_algo      = GCRY_CIPHER_AES256;
constexpr const char* cry_ext                   = ".cry";

constexpr const char* cry_default_password      = "DEFAULTpassword123CHANGEME";
constexpr const char* cry_default_iv            = "cRy-b@by-369";

#include <cry/version.h>

constexpr const char* git_repo        = "ssh://git@git.ignifi.me:10022/deadman/cry.git";
constexpr const char* git_branch      = "master";
constexpr const char* git_hash        = "bad1c0a";
constexpr const uint32_t git_revision       = 284;

#endif // CRY_CONFIG

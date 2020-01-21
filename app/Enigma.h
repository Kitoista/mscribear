// Copyright (c) 2017 Akos Kiss.
//
// Licensed under the BSD 3-Clause License
// <LICENSE.md or https://opensource.org/licenses/BSD-3-Clause>.
// This file may not be copied, modified, or distributed except
// according to those terms.

#include "main.h"

#include "mbed.h"

#include "blue.h"
#include "jrs-thread.h"
#include "Morse.h"

char g_rotors[3][2][26] = {
      {
        {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
        {'e', 'k', 'm', 'f', 'l', 'g', 'd', 'q', 'v', 'z', 'n', 't', 'o', 'w', 'y', 'h', 'x', 'u', 's', 'p', 'a', 'i', 'b', 'r', 'c', 'j'}
      }, {
        {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
        {'a', 'j', 'd', 'k', 's', 'i', 'r', 'u', 'x', 'b', 'l', 'h', 'w', 't', 'm', 'c', 'q', 'g', 'z', 'n', 'p', 'y', 'f', 'v', 'o', 'e'}
      }, {
        {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
        {'b', 'd', 'f', 'h', 'j', 'l', 'c', 'p', 'r', 't', 'x', 'v', 'z', 'n', 'y', 'e', 'i', 'w', 'g', 'a', 'k', 'm', 'u', 's', 'q', 'o'}
      }
    };
char g_reflector[26] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'd', 'i', 'j', 'k', 'g', 'm', 'k', 'm', 'i', 'e', 'b', 'f', 't', 'c', 'v', 'v', 'j', 'a', 't'
    };
char g_config[3] = {'d', 'o', 'g'};

class Enigma {
protected:
  char rotors[3][2][26];
  char reflector[26];
  char config[3];

  int reflect(int index) {
    for (int i = 0; i < 26; ++i) {
      if (i == index) {
        continue;
      }
      if (reflector[i] == reflector[index]) {
        return i;
      }
    }
    printf("you rather have nothing like %d in reflect\n\r", index);
    return -1;
  }

  int shift(int index, int rotorId) {
    return (index+config[rotorId]-'a') % 26;
  }
  int ushift(int index, int rotorId) {
    return (26+index-config[rotorId]+'a') % 26;
  }

  int indexOf(char c, int rotorId, int side) {
    for (int i=0;i<26;++i) {
      int virtualIndex = shift(i, rotorId);
      if (rotors[rotorId][side][virtualIndex] == c) {
        return i;
      }
    }
    printf("you got nothing %c in rotor%d on side%d\n\r", c, rotorId, side);
    return -1;
  }

  char encodeChar(char c) {
    char wip = c;
    int index = c-'a';

    config[2] = (config[2]-'a'+1)%26+'a';

    wip = rotors[2][1][shift(index, 2)];
    index = indexOf(wip, 2, 0);
    wip = rotors[1][1][shift(index, 1)];
    index = indexOf(wip, 1, 0);
    wip = rotors[0][1][shift(index, 0)];
    index = indexOf(wip, 0, 0);

    index = reflect(index);

    wip = rotors[0][0][shift(index, 0)];
    index = indexOf(wip, 0, 1);
    wip = rotors[1][0][shift(index, 1)];
    index = indexOf(wip, 1, 1);
    wip = rotors[2][0][shift(index, 2)];
    index = indexOf(wip, 2, 1);

    return 'a'+index;
  }

public:
  Enigma() {
    memcpy(rotors, g_rotors, sizeof(char)*3*2*26);
    memcpy(reflector, g_reflector, sizeof(char)*26);
    memcpy(config, g_config, sizeof(char)*3);
  }

  void setConfig(char input[3]) {
    config[0] = input[0];
    config[1] = input[1];
    config[2] = input[2];
  }

  void getConfig(char inputConfig[3]) {
    inputConfig[0] = config[0];
    inputConfig[1] = config[1];
    inputConfig[2] = config[2];
  }

  void encode(char *input) {
    for (int i=0;input[i]!=0;++i) {
      input[i] = encodeChar(input[i]);
    }
  }

};

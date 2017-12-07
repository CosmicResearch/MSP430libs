/*
 Bondar libraries - tests
 Copyright (C) 2017  Associació Cosmic Research
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#ifndef SERIAL
#define SERIAL

class Serial {
    
private:
    
    void ((*_onReceive)(error_t));
    void ((*_onSendDone)(void));

public:

    Serial () {}

    void println(char* word) {
        std::cout << word << std::endl;
    }
    
    void begin(uint32_t baudRate) {
        
    }
    
    void attachReceive(void (* func )(uint8_t)) {
        _onReceive = func;
    }
    
    void end() {
        
    }
    
    void detachSendDone() {
        _onSendDone = NULL;
    }
    
    void detachReceive() {
        _onReceive = NULL;
    }
    
    void attachSendDone(void (* func )(void)) {
        _onSendDone = func;
    }


};

#endif

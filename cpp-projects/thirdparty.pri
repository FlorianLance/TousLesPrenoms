

# /*******************************************************************************
# ** TousLesPrenoms                                                             **
# ** MIT License                                                                **
# ** Copyright (c) [2020] [Florian Lance]                                       **
# **                                                                            **
# ** Permission is hereby granted, free of charge, to any person obtaining a    **
# ** copy of this software and associated documentation files (the "Software"), **
# ** to deal in the Software without restriction, including without limitation  **
# ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
# ** and/or sell copies of the Software, and to permit persons to whom the      **
# ** Software is furnished to do so, subject to the following conditions:       **
# **                                                                            **
# ** The above copyright notice and this permission notice shall be included in **
# ** all copies or substantial portions of the Software.                        **
# **                                                                            **
# ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
# ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
# ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
# ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
# ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
# ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
# ** DEALINGS IN THE SOFTWARE.                                                  **
# **                                                                            **
# ********************************************************************************/


########################################################## DIRECTORIES
############################ third-party headers only libraries
CATCH_INCLUDES                       = $$TOUS_LES_PRENOMS_CPP_THIRDPARTY_DIR"/catch"
SIGNALS_INCLUDES                     = $$TOUS_LES_PRENOMS_CPP_THIRDPARTY_DIR"/signals"
############################ third-party others
QWT_DIR                              = $$TOUS_LES_PRENOMS_CPP_THIRDPARTY_DIR"/qwt-6.1.4"

########################################################## INCLUDES

############################# QWT
QWT_INCLUDES = \
    $$QWT_DIR"/src"

########################################################## WINDOWS LIBS
WINDOWS_LIBS = \
    -luser32\
    -lopengl32\
    -lgdi32\
    -lshell32\
    -lws2_32\

equals(COMPILER, "vs"){

########################################################## DEBUG LIBS
    equals(CFG, "debug"){

############################# QWT
        QWT_LIBS =\
            -L$$QWT_DIR"/lib/debug" \
            -lqwtd\
    }

########################################################## RELEASE LIBS
    equals(CFG, "release"){

############################# QWT
        QWT_LIBS =\
            -L$$QWT_DIR"/lib/release" \
            -lqwt\

    }
}

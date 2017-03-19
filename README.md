# IS521 Activity3: Backdoor

## File Description
* [**./interpreter/**](interpreter)
    * [`interpreter.c`](interpreter/interpreter.c) : C source code that reads, translates and executes bytecode which converted by compiler.
* [**./login/**](login)
    * [`login.mini`](login/login.mini) : A login program written in Mini Language. This program is generated by my awesome program [`./generateMini/generateMini.java`](generateMini/generateMini.java).
* [**./test/**](test)
    * [`test.mini`](test/test.mini) : A test program written in Mini Language, which randomly chooses a **donation angel** who will buy others coffee.
    * [`test.md`](test/test.md) : A description file which explains a `test.mini` program in detail.
* [**./backdoor/**](backdoor) : TODO
* [**./compiler/**](compiler)
    * [`compiler.ml`](compiler/compiler.ml) : A compiler written in Ocaml, which convert mini language code into bytecode.
* [**./generateMini/**](generateMini)
    * [`generateMini.java`](generateMini/generateMini.java) : An awesome program written in JAVA, which automatically generates mini source code from the input file. Please refer to the [`./generateMini/README.md`](/generateMini/README.md) for details.


## What I learned
* I learned how compiler understands source code and converts them into bytecode.
* I learned how interpreter handles and executes bytecode.
* When I made a mini source code program by my hand, I realized that I don't have trust in my eyes and fingers. So, I made a program that automatically generates mini source code from the input file. However, I figured out that coding by my hand would be more faster, unless making a new compiler which supports all possible functions.


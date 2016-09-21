assignment1 ross$ gcc copycat.c
assignment1 ross$ ./a.out -b 0 -o output.txt foo2.txt
Error: Buffer size inputted is not valid. Please enter something larger.
assignment1 ross$ ./a.out -b -20 -o output.txt foo2.txt
Error: Buffer size inputted is not valid. Please enter something larger.
assignment1 ross$ ./a.out -b 256 -o output.txt foo2.txt
0.000406
assignment1 ross$ cat output.txt
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
assignment1 ross$ ./a.out -b 256 -o output.txt foo2.txt foo
foo.txt   foo2.txt  foo3.txt
assignment1 ross$ ./a.out -b 256 -o output.txt foo2.txt foo3.txt
0.000335
assignment1 ross$ cat output.txt
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
assignment1 ross$ ./a.out -b 256 foo2.txt foo3.txt
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
0.000077
assignment1 ross$ ./a.out -o output.txt foo2.txt
0.000114
assignment1 ross$ cat output.txt
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
assignment1 ross$ ./a.out foo2.txt
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
0.000057
assignment1 ross$ ./a.out NOT-A-FILE.txt
Error: Function call read failed from NOT-A-FILE.txt. Bad file descriptor.
assignment1 ross$ ./a.out -b 256 - - foo2.txt
hello
hello
world
world
hello world
hello world
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
assignment ross$ ./a.out -b 256 - foo2.txt - foo3.txt -
hi
hi
asldfsjflks
asdlfsdlkfjlsakdj
asflkajsdflsdlkfjlskdjflkasjflksjlfskdjfklasjdlkfjaslfjdslfsdlkfjasdlkfjsldk
asdlfsdlkfj
asdlfsdlkfjasl
middle one
middle one
second line
second line
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
HELLOWORLD
ending
ending
end line
end line
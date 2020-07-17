#!/bin/bash
#     Written by T.Ogihara (C) 2017
# Makes a dependency graph among C source files as a DOT script.
# To display the graph, the "dot" command is needed.
# Example: $ ./makedot.sh | dot -Tpdf -o graph.pdf

if [ $# -eq 0 ]; then
    DIR="."
else
    DIR="$1"
fi
TMP="/tmp/depend$$"
CLIST=`(cd $DIR; /bin/ls -1 *.c)`
LIST=`(cd $DIR; /bin/ls -1 *.{c,h})`
echo 'digraph "graph" {'
for f in $CLIST; do
    echo "    \"$f\" [shape=box];"
done
for f in $LIST; do
    if grep -e '^#\s*include\s*".*"' < "$DIR/$f" > $TMP; then
        awk -F '"' -v "src=$f" '{ printf("    \"%s\" -> \"%s\";\n", src, $2) }' < $TMP
    fi
done
echo '}'
[ -f $TMP ] && /bin/rm -f $TMP
exit 0

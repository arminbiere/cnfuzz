#!/bin/sh

#--------------------------------------------------------------------------#

# comments in 'COMMENTS .. EOF' blocks

COMMENT () {
  echo
  cat
  echo
  echo "[press RETURN to continue or CONTROL-C to abort]"
  read DUMMY </dev/tty
}

# actual executed command in 'EXECUTE .. EOF' blocks

EXECUTE() {
  ( echo "set -x"; cat ) | sh
}

die () {
  echo "*** $NAME: " 1>&2
  exit 1
}

NAME=`basename $0`

checkdir () {
  [ -d $1 ] || die "can not find directory '$1'"
}

checkfile () {
  [ -f $1 ] || die "can not find file '$1'"
}

#--------------------------------------------------------------------------#

COMMENT<<EOF
CNFUZZ and CNFDD Tutorial

This script is executable and at the same time a tutorial.  First let us
get and set up the fuzzing and delta-debugging tool.  You need some tools
installed to make it work, including 'wget', 'unzip', 'tar', 'sed',
'make' and 'gcc', so a standard Unix environment which allows to build
C programs.  Except maybe for 'gcc' and 'make' this should work
out-of-the-box on an Ubuntu installation.

If you are not comfortable running it, just browse the source code
of this script and execute the commands by copying and pasting them.
You can interrupt the script with <CONTROL-C>, but sometimes <CONTROL-Z>
plus executing 'kill %' is necessary, particularly for long running
subprocesses.

The executed code in sub shell scripts is prefixed with '+'.
EOF

EXECUTE<<EOF
rm -f cnfuzzdd2013.zip
wget http://fmv.jku.at/cnfuzzdd/cnfuzzdd2013.zip
unzip cnfuzzdd2013.zip
mv cnfuzzdd2013 cnfuzzdd
cd cnfuzzdd
gcc -O -DNDEBUG -o cnfuzz cnfuzz.c
gcc -O -DNDEBUG -o cnfdd cnfdd.c
cd ..
EOF

checkdir cnfuzzdd
checkfile cnfuzzdd/cnfuzz
checkfile cnfuzzdd/cnfdd

COMMENT<<EOF
If this did not work, maybe it is a good time to press CONTROL-C and
abort this script. Otherwise let us move on and get a SAT solver which
we want to debug.
EOF

EXECUTE<<EOF
wget http://fmv.jku.at/picosat/picosat-960.tar.gz
tar xf picosat-960.tar.gz
cd picosat-960
EOF

checkdir picosat-960

cd picosat-960 # need to change to that directory

COMMENT<<EOF
We plant an artificial bug which will lead to incorrect,
e.g., too large, backjump levels after learning a clause.
EOF

EXECUTE<<EOF
cp picosat.c picosat.c.orig
sed -i -e 's,vlevel < ps->LEVEL,vlevel <= ps->LEVEL,' picosat.c
diff -p picosat.c.orig picosat.c
EOF

COMMENT<<EOF
Then we build this 'buggy' PicoSAT.
EOF

EXECUTE<<EOF
./configure -g
make
EOF

COMMENT<<EOF
We further want to link the compiled tools. In a production
environment you might just want to put them into a directory
in your PATH.
EOF

EXECUTE<<EOF
ln -s ../cnfuzzdd/cnfuzz .
ln -s ../cnfuzzdd/cnfdd .
EOF

COMMENT<<EOF
Now let us generate / fuzz a random instance.  To make the demo
deterministic we fix the random seed for the fuzzer to '0' by
giving it as command line argument.  This will always produce
the same random instance.  Without seed the fuzzer takes system
time and produces a 'real' random instance every time, which
is probably what you want in an actual testing scenario.
EOF

EXECUTE<<EOF
./cnfuzz 0 > bug.cnf
./picosat bug.cnf
EOF

checkfile bug.cnf

COMMENT<<EOF
This should have produced a failed assertion, which shows
that the witness produced by the buggy PicoSAT does not
satisfy the original formula.  This is a sanity check
in PicoSAT which is helpfull to test and debug it.

Let us gather some more information about this failure.
EOF

# have to do this manually because I do not know how
# to escape the $?

echo './picosat bug.cnf 1>&2 >/dev/null; echo $?'
./picosat bug.cnf 1>&2 >/dev/null; echo $?

EXECUTE<<EOF
grep 'p cnf' bug.cnf
EOF

COMMENT<<EOF
First note that the exit code 134 of PicoSAT is not what
you expect from a SAT solver according to the competition
rules, which mandates either 10 or 20.  Actually 0 as
unknown is also fine.  Our CNF delta debugger has these
exit codes hard coded.  That means '0', '10' and '20'
are fine and anything else is considered a failure (bug).

The header as printed by the output of the 'grep' command is

  p cnf 452 3261

which is of considerable size.  So let us delta-debug it.
This may take quite some time (like 10 - 30 seconds).
EOF

EXECUTE<<EOF
./cnfdd bug.cnf reduced.cnf ./picosat
cat reduced.cnf
EOF

checkfile reduced.cnf

COMMENT<<EOF
This should take some seconds and produce something like:

[cnfdd] parsed 452 variables
[cnfdd] parsed 3261 clauses
[cnfdd] this is a propositional instance
[cnfdd] copied 'bug.cnf' to 'reduced.cnf'
[cnfdd] expected exit code without masking out signals is 134
...
<more lines with backspace '^H' and carriage return '^R' deleted>
...
[cnfdd] kept 3 variables
[cnfdd] kept 3 clauses
...
p cnf 3 3
-2 -1 0
2 -3 0
3 -1 0
EOF

cat<<EOF
This is the end of the first demo on using fuzzing and delta-debugging
SAT solvers with 'cnfuzz' and 'cnfdd'.  Maybe it is a good time
to try these tools yourself on your SAT solver now.

To be continued ...

Armin Biere, June 2015.
EOF

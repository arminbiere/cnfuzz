CNF Fuzzer
----------

This is our external CNF fuzzer we have been using since 2007.

External means that we simply fuzz CNF files.  This is enough for
stand-alone solvers (such as [Kissat](https://github.com/arminbiere/kissat),
but in general we consider model based testing through the API as way
superior (as `mobical` does for [CaDiCaL](https://github.com/arminbiere/cadical).

In any case you may want to consider *option fuzzing* and also externalize
as many parameters as possible of your software and turn them into options,
such that they can be fuzzed and particularly delta-debugged.

And while talking about delta-debugging, you might want to checkout
our CNF delta-debugger [cnfdd](https://github.com/arminbiere/cnfdd) too
(used by the scripts in this repository).

<p>
<a href="https://cca.informatik.uni-freiburg.de/papers/BrummayerLonsingBiere-SAT10.pdf">Automated Testing and Debugging of SAT and QBF Solvers</a>
<br>
Robert Brummayer, Florian Lonsing, <a href="https://cca.informatik.uni-freiburg.de/biere/index.html#publications">Armin Biere</a>
<br>
<i>Proc.&nbsp;13th Intl.&nbsp;Conf.&nbsp;on Theory and Applications of Satisfiability Testing (SAT'10)</i>
<br>
<a href="https://www.springer.com/gp/computer-science/lncs">Lecture Notes in Computer Science (LNCS)</a>
<br>
vol.&nbsp;6175,
pages 44-57,
Springer 2010
<br>
[ <a href="https://cca.informatik.uni-freiburg.de/papers/BrummayerLonsingBiere-SAT10.pdf">paper</a>
| <a href="https://cca.informatik.uni-freiburg.de/papers/BrummayerLonsingBiere-SAT10.bib">bibtex</a>
| <a href="https://github.com/arminbiere/cnfuzz">cnfuzz</a>
| <a href="https://github.com/arminbiere/cnfdd">cnfdd</a>
| <a href="https://cca.informatik.uni-freiburg.de/qbfuzz">qbfuzz</a>
| <a href="https://cca.informatik.uni-freiburg.de/qbfdd">qbfdd</a>
]
</p>

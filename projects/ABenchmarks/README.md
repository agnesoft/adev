# ABenchmarks

A collection of benchmarks from other projects measuring the performance sensitive components.

## Set Performance Expectations

Benchmarks without any set expectations are essentially useless. In order to make benchmarks useful set reasonable expectations of performance of your code. For that you will always need at least one alternative to be run and compared against. Most typically you will want to set your code to be simply faster than the alternative. Less commonly you may want your code to perform "well enough" as a factor of the performance of the alternative. The benchmarks are being run on different platforms and you may discover that your solution while being the fastest on one platform does not necessarily perfrom that well on the others. This helps to guide performance tweaks and through the set requirements (test assertions) will maintain the performance over time. Any significant deterioration of the performance in the future, accidental or intentional, should result in a failed benchmark.

# Squares packing


## Useful links

- <https://erich-friedman.github.io/papers/squares/squares.html>


## TODO

- Search: save good quality configurations.
- Add other init positions?


## Issues

- Somehow, clang gives different results even w/o any optimization...
- Results are very sensitive to seeds or small parameter changes.
- Improve on DRAW_SCALE with an adaptative scale.


## Improvements ideas

- improve the search algorithm, make it less greedy?
- improve on the mutation operator. More directed?
- start for a more structured configuration, add some knowledge?
- work on intersections area for a less rigid exploration.
- rng64 for finer position updates?
- SDLA update?

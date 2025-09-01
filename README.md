# Squares packing


## Useful links

- <https://erich-friedman.github.io/papers/squares/squares.html>


## TODO

- Modify createSquare() so it can support any regular polygon (N_SIDES > 3).
- Rename 'square' struct, files to 'polygon'.
- Move the search algorithms to a dedicated file.
- Search: save good quality configurations.
- Add other init positions?


## Issues

- Somehow, clang gives different results even w/o any optimization...
- Results are very sensitive to seeds or small parameter changes.
- Improve on DRAW_SCALE with an adaptative scale.

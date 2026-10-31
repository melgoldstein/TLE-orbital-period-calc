TLE orbital period calculator
Computes two-body orbital dimensions from NORAD two-line elements.
Build: make; regression tests: make test (Python 3).
Run: ./tle_calc sample_leo.txt
Outputs catalog ID, period (minutes), and orbital dimensions (kilometers).
Checks 69-column checksums and matching catalog IDs before calculation.
Uses Earth gravitational parameter 398600.4418 km^3/s^2.
Altitude references the equatorial radius of 6378.137 km.
Mean-element estimates omit perturbations; use SGP4 for propagation.
Requires C99 and libm. Included elements are a numerical fixture. MIT. [Portfolio: melgoldstein.com](https://melgoldstein.com/)

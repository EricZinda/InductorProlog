The Inductor Prolog engine supports only what was required to ship the [Exospecies](www.exospecies.com) game and AI so it is not exhaustive or standards conforming. However, it does have the minimum set of features to right a very robust game AI, so it has most of the base features you'd expect

The following features are for sure not in the Inductor Prolog engine (this is not an exhaustive list):


- Only facts can be asserted or retracted
	- can't declare a function as dynamic like `dynamic(myRule/1)`
		- Anything can be changed in IndProlog, this declaration is not necessary

- ; (or)
- -> (if)
- Terms like a == b instead of ==(a, b)
- " in comments
- _ (unnamed variable)
- Any Metaprogramming features or rules like call
	

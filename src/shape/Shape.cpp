#include "Shape.hpp"

// Provide an out-of-line key function so the vtable/typeinfo are emitted here.
// Define the destructor out-of-line (empty) — do NOT redefine the default
// constructor here (it's implicitly declared).
Shape::~Shape() {}

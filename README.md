# xstructs

Some typed generic data structures, currently only array and linked list.
Requires C11 and `__typeof__` builtin (only for `xarray.h`).

# Files

- `xarray.h`
  Typed generic array that can grow and shrink automatically
- `xlist.h`
  Intrusive linked list using `container_of` semantics. An issue I've found with
  these types of linked lists is that it is easy to mess up what type a link
  belongs to. To prevent this, a macro is used to define a custom link type.

# Usage

Either include the files directly, or use as a meson subproject:
```meson
xstructs_dep = subproject('xstructs').get_variable('xstructs_dep')
```

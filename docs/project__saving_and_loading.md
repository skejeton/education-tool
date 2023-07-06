# Project: Saving and Loading

## Table of Contents

1. [The concept](#the-concept)
2. [Table saver](#table-saver)
3. [Saving and Loading code](#saving-and-loading-code)
4. [Reference](#reference)
5. [Footnotes](#footnotes)

## The concept

This project uses an LBP serializer to save and load the project data.
LBP serializer was invented by developers of Little Big Planet for saving and
loading world data.

Its function is very simple: take the input and do pairwise encoding/decoding
of bytes back to the original data.

For example, if we wanted to encode our value (save it on the disk), here's what
we would do:


```c++
// MODE: Write

int v = 42;
format->pass_value(&v);
```

In this case, the encoder will write the int value sequentially into the buffer.
What if we wanted to decode it? The code is exactly the same:

```c++
// MODE: Read

int v = 42;
format->pass_value(&v);
```

In this case, since `v` is passed as a pointer to `pass_value`, it will write
to `v` the value that was in the encoded document. 


## Table saver

The table saver features an iterator, the usage is as follows:

```cpp
// 1. Create a saver, pass the table of your type by pointer,
//    here, the table type is `Table<int>`.
auto saver = TableSaver<int>::init(encoder, &table);

// 2. Iterate over all elements in the table.
for (;saver.going(); saver.next()) {

  // 3. Save the object and obtain its reference for writing/reading.
  int *object = saver.save();

  // 4. Pass the value of the object.
  format->pass_value(object);
}
```

Let's look at a more complex example, where we pass a string, and a linked list:

```cpp
// 1. Define the object
struct Object {
  int value;
  const char *string;
  TableId next_object;
};

//------------------------------------------------------------------------------

// 2. Create a saver, pass the table of your type by pointer,
//    here, the table type is `Table<Object>`.
auto saver = TableSaver<Object>::init(encoder, &table);

// 3. Iterate over all elements in the table.
for (;saver.going(); saver.next()) {

  // 4. Save the object and obtain its reference for writing/reading.
  Object *object = saver.save();

  // 5. Pass the int field.
  format->pass_value(object->value);

  // 6. Pass the string field.
  format->pass_c_string(object->string);

  // 7. Pass the ID to the next object.
  saver.pass_id(&object->next_object);
}

```

## Saving and Loading code

The saving and loading code is located in `src/project.cpp`, in the 
`save_load_project` function.

## Reference

### `BinaryFormat`

```cpp
static BinaryFormat begin_write()
```
This static member function initializes and returns a `BinaryFormat` object for
writing binary data. It is used to start the serialization process.

```cpp
static BinaryFormat begin_read(void *origin, size_t max)
```
This static member function initializes and returns a `BinaryFormat` object for
reading binary data. It takes in a pointer to the origin of the data (`origin`)
and the maximum size of the data (`max`). It is used to start the
deserialization process.

```cpp
void pass_value(T *data)
```
This member function is used to serialize or deserialize a value of type `T`.
It takes a pointer to the data and performs the necessary encoding/decoding
operations to store or retrieve the value.

```cpp
inline void pass_pointer(void **data, size_t n)
```
This member function is used to serialize or deserialize a pointer to data.
It takes a pointer to the pointer (`data`) and the number of elements (`n`)
being pointed to. It performs the necessary encoding/decoding operations to
store or retrieve the pointer. The memory is allocated on read[^1].

```cpp
inline void pass_c_string(char **string)
```
This member function is used to serialize or deserialize a C-style string
(`char*`). It takes a pointer to the pointer of the string (`string`) and
performs the necessary encoding/decoding operations to store or retrieve
the string. The memory is allocated on read[^1].

```cpp
inline FileBuffer leak_file_buffer()
```
This member function returns a `FileBuffer` object. It is used to obtain a
buffer containing the serialized data. The caller becomes responsible for
managing the memory associated with the returned buffer.


### `TableSaver`

```cpp
static TableSaver init(BinaryFormat *format, Table<T> *table)
```
This static member function initializes and returns a `TableSaver` object.
It takes a pointer to a `BinaryFormat` object (`format`) and a pointer to a
`Table<T>` object (`table`) that needs to be saved. It is used to start the
saving process for the table.

```cpp
bool going()
```
This member function returns a boolean value indicating whether there are more
elements in the table to be saved (`true`) or if the saving process is complete
(`false`).

```cpp
void next()
```
This member function is used to move to the next element in the table during
the saving process.

```cpp
T* save()
```
This member function is used to save the current element in the table. 
It returns a pointer to the saved element of type `T` for reading and writing.

```cpp
void pass_id(TableId *id)
```
This member function is used to serialize or deserialize a `TableId` object.
It takes a pointer to the `TableId` object (`id`) and performs the necessary
encoding/decoding operations to store or retrieve the identifier.

## Footnotes

[^1]: `pass_pointer` and functions using it `pass_string` currently use malloc
  to allocate their pointer data. This is to be fixed with custom allocators.
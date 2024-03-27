This contains the Java bindings for the Substance Connector library, allowing
it to be called from Java.

Coding style:

The brace style will be Allman, with four spaces as the indentation. Class
naming conventions and variable naming conventions shall follow the Java
standard. Method names shall also follow the standard Java convention.
Member and static variables will adopt the m- and s- prefixes from the
integrations coding standard.

Comments will use the standard C block comments for all comments, as to match
the C and C++ code for this project.

Native methods have an exception to the standard - they will use snake\_case
instead of camelCase to match the underlying C standards, as well as to
visually differentiate them from standard methods. These methods should
not be visible in the package API.

Architecture:

The C bindings will dynamically connector with the underlying Substance Connector library.
Runtime path should be set appropriately for the libraries. By keeping this
physical separation, the API is preserved and the underlying C code may be
replaced.

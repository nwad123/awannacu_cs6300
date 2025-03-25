set(LINKED_TO fmt::fmt shared_lib)

new_test(read_input read_input.cpp ${LINKED_TO})
new_test(vec3 vec3.cpp ${LINKED_TO})
new_test(vec2 vec2.cpp ${LINKED_TO})
new_test(bool bool.cpp ${LINKED_TO})

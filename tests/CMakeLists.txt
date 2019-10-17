#g++ -std=c++11 main.cpp -o arkanoid  -lsfml-graphics -lsfml-window -lsfml-system

add_executable(
  test_model 
  test_model.cpp
)

target_link_libraries(test_model  sfml-graphics sfml-audio sfml-window sfml-system)
add_test(NAME test_model COMMAND test_model)

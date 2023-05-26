







// //Тесты пока что не написаны до конца
// #include <string>
// #include <iostream>
// #include <fstream>

// #include "transport_catalogue.h"
// #include "input_reader.h"
// #include "stat_reader.h"

// using namespace std::string_literals;
// using namespace TransportCatalogue;

// template <typename T, typename U>
// void AssertEqualImpl(const T& t, const U& u, const std::string& t_str, const std::string& u_str, const std::string& file,
//     const std::string& func, unsigned line, const std::string& hint) {
//     if (t != u) {
//         std::cout << std::boolalpha;
//         std::cout << file << "("s << line << "): "s << func << ": "s;
//         std::cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
//         std::cout << t << " != "s << u << "."s;
//         if (!hint.empty()) {
//             std::cout << " Hint: "s << hint;
//         }
//         std::cout << std::endl;
//         abort();
//     }
// }

// #define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

// #define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

// void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, unsigned line,
//     const std::string& hint) {
//     if (!value) {
//         std::cout << file << "("s << line << "): "s << func << ": "s;
//         std::cout << "ASSERT("s << expr_str << ") failed."s;
//         if (!hint.empty()) {
//             std::cout << " Hint: "s << hint;
//         }
//         std::cout << std::endl;
//         abort();
//     }
// }

// #define ASSERT(expr) AssertImpl(!!(expr),#expr, __FILE__, __FUNCTION__, __LINE__, ""s)

// #define ASSERT_HINT(expr, hint) AssertImpl(!!(expr),#expr, __FILE__, __FUNCTION__, __LINE__, (hint))


// void TestAddBus(){
//     {
//     TransportCatalogue tc;
//     tc.AddBus("Toronto-Tokyo", {"Toronto", "Tokyo"});
//     ASSERT(tc.GetBus("Toronto-Tokyo") != nullptr); // маршрут добавлен
//     ASSERT(tc.GetStopCount() == 2); //неинициализированные ранее остановки добавлены
//     ASSERT(tc.GetBusCount() == 1); //добавлен только один маршрут
//     }
//     {
//     TransportCatalogue tc;
//     tc.AddBus("Toronto Tokyo", {"Toronto", "Tokyo"});
//     ASSERT(tc.GetBus("Toronto Tokyo") != nullptr); // в названии маршрута пробел
//     }
//     {
//     TransportCatalogue tc;
//     tc.AddBus("Toronto Tokyo", {"Toronto", "Tokyo", "Toronto"});
//     ASSERT(tc.GetStopCount() == 2); // существующая неинициализировання остановка не добавлена повторно
//     }
// }

// void TestAddStop(){
//     {
//     TransportCatalogue tc;
//     tc.AddStop("Toronto", {55.611087, 37.208290});
//     tc.AddStop("Tokyo", {57.611087, 27.208290});
//     ASSERT(tc.GetStopCount() == 2); //остановки добавлены
//     }
//     {
//     TransportCatalogue::TransportCatalogue tc;
//     tc.AddStop("Toronto", {55.611087, 37.208290});
//     tc.AddStop("Toronto", {55.611088, 37.208290});
//     ASSERT(tc.GetStopCount() == 1); //повторяющаяся остановка не добавлена
//     }
// }

// void TestIReader(){
//     {
//     std::ifstream stream("/tests/test1.txt");
//     TransportCatalogue::TransportCatalogue tc;
//     IReader Iread(stream);
//     Iread.ReadInput(tc);
//     stream.close();
//     ASSERT_EQUAL(tc.GetStopCount(), 8); //остановки добавлены верно
//     ASSERT(tc.GetBus("1") != nullptr); // марщрут добавлен верно / имя маршрута верное
//     ASSERT(tc.GetBus("2") != nullptr); // последняя строка-маршрут обрабатывается
//     BusInfo bus1 = tc.GetBusInfo("1");
//     ASSERT(bus1.all_count = 6); // маршрут простой по кайфу работает
//     ASSERT(bus1.unique_count = 5); //не считает повторяющиеся остановки
//     BusInfo bus2 = tc.GetBusInfo("2");
//     ASSERT(bus2.all_count = 5); //кольцевой маршрут обработан верно
//     ASSERT(bus2.unique_count = 3); // кольцевой маршрут верно идет
//     }
//     {
//     std::ifstream stream("/tests/test1.txt");
//     TransportCatalogue tc;
//     IReader Iread(stream);
//     stream.close();
//     ASSERT(tc.GetStopCount() == 8); //остановки добавлены верно
//     ASSERT(tc.GetBus("1") != nullptr); // марщрут добавлен верно / имя маршрута верное
//     ASSERT(tc.GetBus("2") != nullptr); // последняя строка-маршрут обрабатывается
//     BusInfo bus1 = tc.GetBusInfo("1");
//     ASSERT(bus1.all_count = 6); // маршрут простой по кайфу работает
//     ASSERT(bus1.unique_count = 5); //не считает повторяющиеся остановки
//     BusInfo bus2 = tc.GetBusInfo("2");
//     ASSERT(bus2.all_count = 5); //кольцевой маршрут обработан верно
//     ASSERT(bus2.unique_count = 3); // кольцевой маршрут верно идет
//     ASSERT_EQUAL(bus2.length, 20939.5);
//     }
    
// }

// // void TestSReader(){
// //     {
// //     std::ifstream stream("/tests/test1.txt");
// //     TransportCatalogue tc;
// //     IReader Iread(stream);
// //     SReader SRead
// //     stream.close();
// //     }
    
// // }


// void RunTest(){
//     std::cout << "Test runned!" << std::endl;
//     TestAddBus();
//     std::cout << "ADDBYS" << std::endl;
//     TestAddStop();
//     //TestIReader();
//     std::cout << "GOOD!" << std::endl;
// }
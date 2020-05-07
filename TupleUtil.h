#ifndef UTIL_H_
#define UTIL_H_

#include <tuple>
#include <sstream>

using namespace std;

//============ Tuple comparing =============

template<bool flag, size_t N1, size_t N2>
struct MaxSelector {
    enum {
        maxValue = N1
    };
};

template<size_t N1, size_t N2>
struct MaxSelector<false, N1, N2> {
    enum {
        maxValue = N2
    };
};

template<typename Tuple1, typename Tuple2, size_t N>
struct ComparingHelper {
    static int compare(Tuple1 const &t1, Tuple2 const &t2) {
        if constexpr(N != 1) {
            return middleCompare(t1, t2);
        } else {
            return lastCompare(t1, t2);
        }
    }

    static int customCompare(Tuple1 const &t1, Tuple2 const &t2, int &levelDifference) {

        if constexpr (N != 1) {
            return middleCustomCompare(t1, t2, levelDifference);
        } else {
            return lastCustomCompare(t1, t2, levelDifference);
        }
    }

private:

    static int middleCustomCompare(Tuple1 const &t1, Tuple2 const &t2, int &levelDifference) {
        int isCompareNext = ComparingHelper<Tuple1, Tuple2, N - 1>::customCompare(t1, t2, levelDifference);
        if (isCompareNext) {
            if (get<N - 1>(t1) == get<N - 1>(t2)) {
                ++levelDifference;
                return true;
            }
        }
        return false;
    }

    static int lastCustomCompare(Tuple1 const &t1, Tuple2 const &t2, int &levelDifference) {
        if (get<0>(t1) == get<0>(t2)) {
            levelDifference++;
            return true;
        }
        return false;
    }

    static int middleCompare(Tuple1 const &t1, Tuple2 const &t2) {
        int result = ComparingHelper<Tuple1, Tuple2, N - 1>::compare(t1, t2);
        if (result == 0) {
            if (get<N - 1>(t1) > get<N - 1>(t2)) {
                result = 1;
            } else if (get<N - 1>(t1) < get<N - 1>(t2)) {
                result = -1;
            } else {
                result = 0;
            }
        }
        return result;
    }

    static int lastCompare(Tuple1 const &t1, Tuple2 const &t2) {
        if (get<0>(t1) > get<0>(t2)) {
            return 1;
        } else if (get<0>(t1) < get<0>(t2)) {
            return -1;
        } else {
            return 0;
        }
    }

};

template<typename... Args1, typename... Args2>
int compare(tuple<Args1...> const &t1,
            tuple<Args2...> const &t2) {
    return
            ComparingHelper<decltype(t1), decltype(t2),
                    MaxSelector<sizeof...(Args1) < sizeof...(Args2),
                            sizeof...(Args1), sizeof...(Args2)>::maxValue>::compare(t1, t2);
}

template<typename... Args1, typename... Args2>
int customCompare(tuple<Args1...> const &t1,
                  tuple<Args2...> const &t2,
                  int &levelDifference) {
    return
            ComparingHelper<decltype(t1), decltype(t2),
                    MaxSelector<sizeof...(Args1) < sizeof...(Args2),
                            sizeof...(Args1), sizeof...(Args2)>::maxValue>::customCompare(t1, t2, levelDifference);
}

//======== Tuple Out ============

template<class Tuple, size_t N>
struct TupleWriter {
    static ostream &write(ostream &os, Tuple const &tuple) {
        TupleWriter<Tuple, N - 1>::write(os, tuple);
        os << " " << get<N - 1>(tuple);
        return os;
    }
};

template<class Tuple>
struct TupleWriter<Tuple, 1> {
    static ostream &write(ostream &os, Tuple const &tuple) {
        os << get<0>(tuple);
        return os;
    }
};

template<class CharT, class Traits, class... Types>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &out,
                                              tuple<Types...> const &outputTuple) {
    return TupleWriter<decltype(outputTuple), tuple_size<tuple<Types...>>::value>::write(out, outputTuple);
}

//======== Tuple In ============

template<class CharT, class Traits, class HeadType, class... TailTypes>
std::basic_istream<CharT, Traits> &operator>>(std::basic_istream<CharT, Traits> &in,
                                              tuple<HeadType, TailTypes...> &inputTuple) {

    HeadType inputValue;
    tuple<TailTypes...> restInput;

    in >> inputValue;
    in >> restInput;

    inputTuple = tuple_cat(make_tuple(inputValue), restInput);
    return in;
}

template<class CharT, class Traits>
std::basic_istream<CharT, Traits> &operator>>(std::basic_istream<CharT, Traits> &in, std::tuple<> &tuple) {
    return in;
}

#endif
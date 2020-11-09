//
// Created by kookie on 30. 10. 2020..
//

#ifndef KKI_UTIL_STRING_H
#define KKI_UTIL_STRING_H

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <cstring>
#include <string>
#include <ostream>
#include <functional>
#include "util.h"

namespace kki
{
    class string_builder;

    class string{
    public:

        // ================= //
        // Basic definitions //
        // ================= //

        using iterator = const char *;
        static const size_t npos = -1;

        // ============ //
        // Constructors //
        // ============ //

        string() : string(""){}
        string(const char *cstr) : string(length(cstr), cstr){}
        string(size_t len, const char *data) {
            container = make_ref<std::vector<char>>();
            container->reserve(len + 1);
            std::copy(data, data + len, std::back_inserter(*container));
            container->emplace_back('\0');

            _begin  = container->data();
            _end    = container->data() + len;
        }
        // Format consructor
        template<typename ...T_args>
        string(const char *format, T_args ...args);

        // ================ //
        // Basic operations //
        // ================ //

        inline const char* data() const {
            return _begin;
        }
        inline iterator begin() const{
            return _begin;
        }
        inline iterator end() const{
            return _end;
        }
        inline size_t size() const{
            return _end - _begin;
        }

        void set_data(const ref<std::vector<char>>& data){
            container = data;
            _begin = container->data();
            _end = _begin + container->size();
        }

        // ==== //
        // Hash //
        // ==== //

        constexpr static size_t hash(const char* data){
            const size_t p = 131, m = 4294967291; // 2^32 - 5, largest 32 bit prime
            size_t total = 0, current_multiplier = 1;

            for (size_t i = 0; data[i] != '\0'; ++i){
                total = (total + current_multiplier * data[i]) % m;
                current_multiplier = (current_multiplier * p) % m;
            }
            return total;
        }
        static size_t hash(const char* data, size_t len){
            const size_t p = 131, m = 4294967291; // 2^32 - 5, largest 32 bit prime
            size_t total = 0, current_multiplier = 1;

            for (size_t i = 0; i < len; ++i){
                total = (total + current_multiplier * data[i]) % m;
                current_multiplier = (current_multiplier * p) % m;
            }
            return total;
        }
        size_t hash() const{
            return hash(begin(), size());
        }

        // ========= //
        // Operators //
        // ========= //

        inline char operator [](size_t i) const{
            assert(i < size());
            return *(begin() + i);
        }

        // ========== //
        // Comparison //
        // ========== //

        inline bool operator ==(const char* other) const {
            size_t len = length(other);
            return size() == len && equal(data(), other, len);
        }
        inline bool operator ==(const string& other) const {
            return size() == other.size() && (data() == other.data() || equal(data(), other.data(), size()));
        }

        inline bool operator !=(const char* other) const {
            return !(*this == other);
        }
        inline bool operator !=(const string& other) const {
            return !(*this == other);
        }

        inline bool operator <(const char* other) const {
            return compare(data(), other, std::min(size(), length(other))) < 0;
        }
        inline bool operator <(const string& other) const {
            return compare(data(), other.data(), std::min(size(), other.size())) < 0;
        }

        inline bool operator <=(const char* other) const {
            return compare(data(), other, std::min(size(), length(other))) <= 0;
        }
        inline bool operator <=(const string& other) const {
            return compare(data(), other.data(), std::min(size(), other.size())) <= 0;
        }

        inline bool operator >(const char* other) const {
            return compare(data(), other, std::min(size(), length(other))) > 0;
        }
        inline bool operator >(const string& other) const {
            return compare(data(), other.data(), std::min(size(), other.size())) > 0;
        }

        inline bool operator >=(const char* other) const {
            return compare(data(), other, std::min(size(), length(other))) >= 0;
        }
        inline bool operator >=(const string& other) const {
            return compare(data(), other.data(), std::min(size(), other.size())) >= 0;
        }

        // ======== //
        // Addition //
        // ======== //

        template<typename T_add>
        string_builder operator+(T_add other);
        template<typename T_add>
        void operator+=(const T_add& other);

        // ============== //
        // Multiplication //
        // ============== //

        string_builder operator*(size_t _i);
        void operator*=(size_t _i);

        // ====== //
        // Search //
        // ====== //

        // TODO: Add Boyer-Moore search

        // Find
        size_t find(char element, size_t start = 0) const{
            assert(begin() + start <= end());
            auto res = find_ptr(begin() + start, end(), element);
            return res == end() ? npos : res-begin();
        }
        size_t find(const char* element, size_t start = 0) const{
            return find(element, length(element), start);
        }
        size_t find(const string& element, size_t start = 0) const{
            return find(element.begin(), element.size(), start);
        }
        size_t find(const char* element, size_t len, size_t start) const{
            const char* pos = find_ptr(begin() + start, end(), element, len);
            return pos == end() ? npos : pos - begin();
        }

        // Find if
        template<typename T_pred>
        size_t find_if(const T_pred& predicate, size_t start = 0) const{
            const char* current = data() + start;
            assert(current <= end());
            while(current < end() && !predicate(*current))
                current++;

            return current == end() ? npos : current-begin();
        }

        // Find all instances
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(char element, size_t start = 0, size_t elements = 0) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find(element, start);
            while (current_pos != npos){
                res.push_back(current_pos);
                current_pos = find(element, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const string& element, size_t start = 0, size_t elements = 0) const{
            return find_all(element.begin(), element.size(), start, elements);
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const char* element, size_t start = 0, size_t elements = 0) const{
            return find_all(element, length(element), start, elements);
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const char* element, size_t len, size_t start, size_t elements) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find(element, len, start);
            while (current_pos != npos){
                res.push_back(current_pos);
                current_pos = find(element, len, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }

        // Find all instances conditional
        template<typename T_alloc=std::allocator<size_t>, typename T_pred>
        std::vector<size_t, T_alloc> find_all_if(const T_pred& pred, size_t start = 0, size_t elements = 0) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find_if(pred, start);
            while (current_pos != npos ){
                res.push_back(current_pos);
                current_pos = find_if(pred, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }

        // ==== //
        // Trim //
        // ==== //

        // Right trim
        string& r_trim(){
            while (_end != begin() && isspace(*(_end - 1)))
                --_end;
            return *this;
        }
        string r_trimmed() const {
            const char* new_end = data() + size();
            while (new_end != begin() && isspace(*(new_end - 1)))
                --new_end;
            return string(container, begin(), new_end);
        }
        // Left trim
        string& l_trim(){
            while (_begin != _end && isspace(*_begin))
                ++_begin;
            return *this;
        }
        string l_trimmed() const {
            const char* new_begin = data();
            while (new_begin != _end && isspace(*new_begin))
                ++new_begin;
            return string(container, new_begin, end());
        }
        // Trim both sides
        string& trim(){
            l_trim();
            r_trim();
            return *this;
        }
        string trimmed() const {
            return l_trimmed().r_trim();
        }

        // ===== //
        // Apply //
        // ===== //

        // Converts string using a given predicate
        string& apply(const std::function<char(char)>& predicate){
            // Try to detach the string so it is the only string that uses the memory
            detach();
            // Fetch variables for iteration
            char* b = container->data() + (begin() - container->data());
            char* e = container->data() + (end() - container->data());
            // Iterate over all data
            for (char* i = b; i != e; ++i){
                *i = predicate(*i);
            }
            return *this;
        }

        string applied(const std::function<char(char)>& predicate) const {
            string new_string = clone();
            new_string.apply(predicate);
            return new_string;
        }

        // ===== //
        // Split //
        // ===== //

        // Split string with the delimiter being one or more whitespaces
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<string, T_alloc> split() const{
            std::vector<string, T_alloc> res;
            iterator current = begin();
            while (true){
                // Find begin
                while (current != end() &&  isspace(*current))++current;
                if(current == end())break;
                iterator start = current;
                while (current != end() && !isspace(*current))++current;
                res.push_back({container, start, current});
                if(current == end())break;
            }
            return res;
        }
        // Split string with custom char delimiter
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<string, T_alloc> split(char delimiter, size_t splits=0) const{
            std::vector<string, T_alloc> res;
            res.reserve(splits);
            iterator current = begin();
            if((*current) == delimiter){
                res.push_back({container, current, current});
            }
            while (current < end()){
                iterator next = find_ptr(current + 1, end(), delimiter);
                res.push_back({container, current, next});
                current = next;
            }
            res.shrink_to_fit();
            return res;
        }
        // Split string with any version of string
        std::vector<string> split(const char* delimiter, size_t splits = 0) const{
            return split(delimiter, length(delimiter), splits);
        }
        std::vector<string> split(const string& delimiter, size_t splits = 0) const{
            return split(delimiter.begin(), delimiter.size(), splits);
        }
        std::vector<string> split(const char* delimiter, size_t len, size_t splits) const{
            std::vector<string> res;
            res.reserve(splits);
            iterator current = begin();

            while (current < end()){
                iterator next = find_ptr(current, end(), delimiter, len);
                res.push_back({container, current, next});
                current = next + len;
            }

            if (equal(end() - len, delimiter, len)){
                res.push_back({container, end(), end()});
            }
            res.shrink_to_fit();
            return res;
        }

        // ========== //
        // Substrings //
        // ========== //

        string substr(size_t first, size_t n=npos, bool use_len=true){
            const char* _first = begin() + first;
            const char* _last = nullptr;
            if(use_len){
                _last = n != npos ? _first + n : end();
            } else {
                _last = n != npos ? begin() + n : end();
            }
            assert(begin() <= _first);
            assert(_first < _last);
            assert(_first <= end());
            return string{container, _first, _last};
        }
        string operator()(size_t first, size_t end){
            return substr(first, end, false);
        }
        string substr(const char *first, const char *last=nullptr){
            last = last != nullptr ? last : end();
            assert(begin() <= first);
            assert(first < last);
            assert(last <= end());

            return string{container, first, last};
        }

        // ================= //
        // Memory management //
        // ================= //

        // Detach the string from the original memory
        // If the string is the only string that uses that container there is no reason to detach
        // If the string is sharing the container, a new container reference of fitting size is created
        // and the content of the string is copied and zero terminated
        // Return: returns true if the string has been detached
        bool detach(){
            // Check if the container is in use by any other string
            if (container.use_count() != 1){
                // Get the current size of the string
                size_t len = size();
                // Create new container
                auto res = make_ref<std::vector<char>>();

                res->reserve(len + 1);
                // Copy contents and zero terminate
                std::copy(begin(), end(), std::back_inserter(*res));
                res->push_back('\0');
                // Set member variables
                container = res;
                _begin = container->data();
                _end = begin() + len;
                return true;
            }
            return false;
        }

        // Return: returns true if string is of minimal size
        bool shrink(){
            std::vector<char>& data = *container;

            // Check if the size of the container is minimal
            // -1 because of the zero terminated string
            if ((data.size() - 1) != size()){
                // Check if it is the only string that uses the container
                if (container.use_count() == 1){
                    size_t len = size();
                    // If the data is not at the start of the container, copy it to the start
                    if (begin() != data.data()){
                        std::copy(begin(), end(), data.begin());
                    }
                    // Zero terminate the string
                    data[len] = '\0';
                    // Shrink the string to the new size
                    data.resize(len + 1);
                    // Reset the pointers to the begining and end of string
                    _begin = data.data();
                    _end = _begin + len;
                    return true;
                }
                return false;
            }
            // Size of container is minima
            return true;
        }

        // Return c style null terminated pointer
        const char* cstr(){
            std::vector<char>& data = *container;
            // If the string is not already null terminated process string
            if ((*end()) != '\0'){
                // If the string is the only one that uses the container zero terminate it in place
                if (container.use_count() == 1){
                    // Calculate offset from the start of container data to start of string
                    size_t offset = begin() - data.data();
                    // Set the value at the end of the container to \0
                    data[offset + size()] = '\0';
                } else {
                    // If it is not the only string that uses the same container
                    // Detach from the container
                    // The zero terminator is added in the detach function
                    detach();
                }
            }
            return _begin;
        }

        string clone() const {
            ref<std::vector<char>> new_container = make_ref<std::vector<char>>();
            new_container->reserve(size());
            std::copy(begin(), end(), std::back_inserter(*new_container));
            return {new_container, new_container->data(), new_container->data() + new_container->size()};
        }

        // ==== //
        // Cast //
        // ==== //

        operator bool() const{
            return size() != 0;
        }

        operator const char*() {
            return cstr();
        }

        // ==== //
        // UTIL //
        // ==== //

        static const char* find_ptr(const char* start, const char* end, char element){
            auto res = static_cast<const char*>(memchr(start, element, end - start));
            return res == nullptr ? end : res;
        }
        static const char* find_ptr(const char* start, const char* end, const char* element, size_t len){
            switch (len) {
                case 0:
                    return start;
                case 1:
                    return find_ptr(start, end, element[0]);
                default:;
            }

            const char* current = start;
            assert(current <= end);
            const char* last = end - len;
            while (true){
                current = find_ptr(current, end, element[0]);
                if (current > last){
                    return end;
                }
                ++current;
                if (equal(current, element + 1, len - 1)){
                    return current - 1;
                }
            }
        }

        template<typename T_stream>
        bool getline(T_stream& input_stream){
            static std::string line;
            bool res = static_cast<bool>(std::getline(input_stream, line));
            detach();
            container->clear();
            container->reserve(line.length());
            std::copy(line.begin(), line.end(), std::back_inserter(*container));
            container->push_back('\0');
            _begin = container->data();
            _end = _begin + line.length();
            return res;
        }

        static bool equal(const char* i1, const char* i2, size_t len){
            return memcmp(i1, i2, len) == 0;
        }
        static int compare(const char* i1, const char* i2, size_t len){
            return memcmp(i1, i2, len);
        }
        static inline size_t length(const char* cstr){
            return strlen(cstr);
        }

    private:

        // Private constructor for member functions
        string(const ref<std::vector<char>>& data, const char* begin, const char* end) : _begin(begin), _end(end), container(data){}

        const char* _begin{nullptr}, *_end{nullptr};
        ref<std::vector<char>> container;
    };

    // TODO:
    //      Adding different kinds of containers to print
    class string_builder{
    public:
        using iterator = char*;
        static const size_t npos = string::npos;

        // ============ //
        // Constructors //
        // ============ //

        explicit string_builder(size_t size=0) : _data(make_ref<std::vector<char>>()){
            _data->reserve(size);
        }
        string_builder(char t, size_t size) : _data(make_ref<std::vector<char>>()){
            _data->reserve(size);
            std::fill_n(std::back_inserter(*_data), size, t);
        }
        explicit string_builder(const char* cstr) : string_builder(string::length(cstr), cstr){}
        explicit string_builder(const string& string) : string_builder(string.size(), string.begin()){}
        string_builder(size_t len, const char* data) : _data(make_ref<std::vector<char>>()){
            _data->reserve(len);
            std::copy(data, data + len, std::back_inserter(*_data));
        }
        explicit string_builder(ref<std::vector<char>>& data_container) : _data(data_container){}
        template<typename ...T_args>
        explicit string_builder(const char *format, T_args ...args) : _data(make_ref<std::vector<char>>()){
            format_recursion(*this, format, args...);
        }

        // ================== //
        // Data getter/setter //
        // ================== //

        void set_data_container(ref<std::vector<char>>& other){
            _data = other;
        }
        ref<std::vector<char>> get_data_container()
        {
            return _data;
        }

        // =============== //
        // Basic functions //
        // =============== //

        inline size_t size() const{
            return _data->size();
        }
        inline size_t capacity() const{
            return _data->capacity();
        }
        inline void reserve(size_t size){
            _data->reserve(size);
        }
        inline char* data(){
            return _data->data();
        }
        inline const char* data() const{
            return _data->data();
        }
        inline iterator begin() const {
            return _data->data();
        }
        inline iterator end() const {
            return _data->data() + _data->size();
        }
        inline char& back(){
            return _data->back();
        }

        // ============= //
        // getter/setter //
        // ============= //

        inline char& at(size_t i){
            assert(i < _data->size());
            return _data->at(i);
        }
        inline char  get(size_t i) const{
            assert(i < _data->size());
            return _data->at(i);
        }
        inline void  set(size_t i, char elem){
            assert(i < _data->size());
            _data->at(i) = elem;
        }

        inline char& operator[](size_t i){
            return at(i);
        }
        inline char  operator[](size_t i) const{
            return get(i);
        }

        // ==================== //
        // String manipulations //
        // ==================== //

        string_builder& append(const char* c, size_t len){
            // So it doesnt reserve for too small strings
            if(len > _data->capacity()){
                _data->reserve(_data->capacity() + len);
            }
            std::copy(c, c+len, std::back_inserter(*_data));
            return *this;
        }
        template<typename T_pr>
        string_builder& append(T_pr t){
            return (*this) << t;
        }

        string_builder& operator<<(const char* c){
            return append(c, string::length(c));
        }
        string_builder& operator<<(const string& c){
            return append(c.begin(), c.size());
        }
        string_builder& operator<<(bool b){
            return append(b ? "true" : "false");
        }
        string_builder& operator<<(char c){
            _data->push_back(c);
            return *this;
        }
        string_builder& operator<<(int i){
            size_t len = sprintf(buf, "%d", i);
            return append(buf, len);
        }
        string_builder& operator<<(long i){
            size_t len = sprintf(buf, "%li", i);
            return append(buf, len);
        }
        string_builder& operator<<(long long i){
            size_t len = sprintf(buf, "%lli", i);
            return append(buf, len);
        }
        string_builder& operator<<(float f){
            size_t len = sprintf(buf, "%f", f);
            return append(buf, len);
        }
        string_builder& operator<<(double d){
            size_t len = sprintf(buf, "%lf", d);
            return append(buf, len);
        }
        string_builder& operator<<(unsigned u){
            size_t len = sprintf(buf, "%u", u);
            return append(buf, len);
        }
        string_builder& operator<<(unsigned long u){
            size_t len = sprintf(buf, "%lu", u);
            return append(buf, len);
        }
        string_builder& operator<<(unsigned long long u){
            size_t len = sprintf(buf, "%llu", u);
            return append(buf, len);
        }

        string_builder operator*(size_t _i){
            string_builder s(_data->size() * _i);
            for(size_t i = 0; i < _i; ++i){
                std::copy(_data->begin(), _data->end(), std::back_inserter(*s._data));
            }
            return s;
        }
        string_builder& operator*=(size_t _i){
            _data->resize(_data->size() * _i);
            for(size_t i = 0; i < _i - 1; ++i){
                std::copy(_data->begin(), _data->end(), std::back_inserter(*_data));
            }
            return *this;
        }

        template<typename T_p>
        string_builder operator+(T_p p){
            string_builder s;
            return s.append(_data->data(), _data->size()) << append(p);
        }
        template<typename T_p>
        string_builder& operator+=(T_p p){
            return this->append(p);
        }

        // ==================== //
        // Conversion to string //
        // ==================== //

        string to_string(){
            return string(_data->size(), _data->data());
        }
        operator string(){
            return to_string();
        }

        // ====== //
        // Format //
        // ====== //

        template<typename... T_args>
        static string format(const char* format, T_args... args){
            string_builder b;
            format_recursion(b, format, args...);
            return b.to_string();
        }

        // ==== //
        // Find //
        // ==== //

        // Find
        size_t find(char element, size_t start = 0) const{
            assert(data() + start <= end());
            auto res = string::find_ptr(begin() + start, end(), element);
            return res == end() ? npos : res-begin();
        }
        size_t find(const char* element, size_t start = 0) const{
            return find(element, string::length(element), start);
        }
        size_t find(const string& element, size_t start = 0) const{
            return find(element.begin(), element.size(), start);
        }
        size_t find(const char* element, size_t len, size_t start) const{
            const char* pos = string::find_ptr(begin() + start, end(), element, len);
            return pos == end() ? npos : pos - begin();
        }

        // Find if
        template<typename T_pred>
        size_t find_if(const T_pred& predicate, size_t start = 0) const{
            const char* current = data() + start;
            assert(current <= end());
            while(current < end() && !predicate(*current))
                current++;

            return current == end() ? npos : current-begin();
        }

        // Find all instances
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(char element, size_t start = 0, size_t elements = 0) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find(element, start);
            while (current_pos != npos){
                res.push_back(current_pos);
                current_pos = find(element, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const string& element, size_t start = 0, size_t elements = 0) const{
            return find_all(element.begin(), element.size(), start, elements);
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const char* element, size_t start = 0, size_t elements = 0) const{
            return find_all(element, string::length(element), start, elements);
        }
        template<typename T_alloc=std::allocator<size_t>>
        std::vector<size_t, T_alloc> find_all(const char* element, size_t len, size_t start, size_t elements) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find(element, len, start);
            while (current_pos != npos){
                res.push_back(current_pos);
                current_pos = find(element, len, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }

        // Find all instances conditional
        template<typename T_alloc=std::allocator<size_t>, typename T_pred>
        std::vector<size_t, T_alloc> find_all_if(const T_pred& pred, size_t start = 0, size_t elements = 0) const{
            std::vector<size_t, T_alloc> res;
            res.reserve(elements);
            size_t current_pos = find_if(pred, start);
            while (current_pos != npos ){
                res.push_back(current_pos);
                current_pos = find_if(pred, current_pos + 1);
            }
            res.shrink_to_fit();
            return res;
        }

        struct view{
        public:
            char& at(size_t i){
                return _parent.at(_begin + i);
            }
            char& operator[](size_t i){
                return at(i);
            }

            char* begin(){
                return _parent.begin() + _begin;
            }
            char* end(){
                return _parent.begin() + _end;
            }
            size_t size() const{
                return _end - _begin;
            }

            template<typename T_pr>
            string_builder& operator=(const T_pr& i){
                string_builder other;
                other << i;
                size_t len = other.size();
                if(len < size()){
                    // Replacement string is shorter than the view
                    std::copy(other.begin(), other.end(), begin());
                    _parent._data->erase(_parent._data->begin() + _begin + len, _parent._data->begin() + _end);
                }
                else if(len >= size()) {
                    // Replacement string is longer than the view
                    std::copy(other.begin(), other.begin() + size(), begin());
                    _parent._data->insert(_parent._data->begin() + _begin + size(), other.begin() + size(), other.end());
                }
                else{
                    // Replacement string is of equal size
                    std::copy(other.begin(), other.end(), begin());
                }
                // Update the end of the view to include the whole new string
                _end = _begin + len;
                return _parent;
            }

        private:
            friend string_builder;
            view(string_builder& parent, size_t begin, size_t end) : _parent{parent}, _begin{begin}, _end{end}{
                assert(begin <= end);
                assert(end <= parent._data->size());
            }

            string_builder& _parent;
            size_t _begin;
            size_t _end;
        };

        // Inserter view;
        view operator()(size_t begin){
            return {*this, begin, begin};
        }

        view operator()(size_t begin, size_t n, bool use_len = true){
            return {*this, begin, use_len ? (begin + n) : n};
        }

    private:

        // Format recursion functions
        static void format_recursion(string_builder& b, const char* format){
            b << format;
        }

        template<typename T, typename... T_args>
        static void format_recursion(string_builder& b, const char* format, T value, T_args... args){
            for (;(*(format+1)) != '\0'; ++format){
                if ((*format) == '{' && (*(format+1)) == '}'){
                    b << value;
                    format_recursion(b, format+2, args...);
                    return;
                }
                b << *format;
            }
        }

        char buf[128]{};
        ref<std::vector<char>> _data;
    };

    // Formatted string constructor
    template<typename... T_args>
    string::string(const char *format, T_args... args) {
        string s = string_builder::format(format, args...);
        this->container = s.container;
        this->_begin = s.begin();
        this->_end = s.end();
    }

    template<typename T_add>
    string_builder string::operator+(T_add other) {
        string_builder s(*this);
        return s << other;
    }

    template<typename T_add>
    void string::operator+=(const T_add& other) {
        string_builder sb(*this);
        sb << other;
        string new_str = sb.to_string();
        this->container = new_str.container;
    }

    string_builder string::operator*(size_t _i) {
        string_builder sb(this->size() * _i);
        for (size_t i = 0; i < _i; ++i){
            sb << (*this);
        }
        return sb;
    }

    void string::operator*=(size_t _i) {
        string_builder sb(this->size() * _i);
        for (size_t i = 0; i < _i; ++i){
            sb << (*this);
        }
        string new_str = sb.to_string();
        this->container = new_str.container;
    }

    // Util

    template<typename T_stream>
    static string getline(T_stream& input_stream){
        static std::string line;
        std::getline(input_stream, line);
        return string(line.size(), line.data());
    }

    template<typename T_stream>
    static bool getline(T_stream& input_stream, string& str){
        static std::string line;
        bool res = static_cast<bool>(std::getline(input_stream, line));
        str = string(line.size(), line.data());
        return res;
    }

}

std::istream& operator >>(std::istream& stream, kki::string& string){
    string.getline(stream);
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const kki::string& string){
    return stream.write(string.begin(), string.size());
}

std::ostream& operator <<(std::ostream& stream, const kki::string_builder& builder){
    return stream.write(builder.data(), builder.size());
}

namespace std {
    template <>
    struct hash<kki::string>
    {
        std::size_t operator()(const kki::string& k) const
        {
            return k.hash();
        }
    };
}


#endif //KKI_UTIL_STRING_H

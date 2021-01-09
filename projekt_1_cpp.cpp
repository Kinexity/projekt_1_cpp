#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/multiprecision/cpp_int.hpp>
#include <chrono>

class C_Time_Counter {
private:
	std::chrono::high_resolution_clock::time_point
		beg,
		end;
	bool
		counting_started = false;
public:
	C_Time_Counter() = default;
	~C_Time_Counter() = default;
	inline void
		start(),
		stop();
	inline std::chrono::duration<double, std::ratio<1, 1>>
		measured_timespan() noexcept;
};

inline void C_Time_Counter::start() {
	if (counting_started) {
		throw std::runtime_error("Counting already started!");
	}
	else {
		beg = std::chrono::high_resolution_clock::now();
		counting_started = true;
	}
}

inline void C_Time_Counter::stop() {
	if (!counting_started) {
		throw std::runtime_error("Counting not started!");
	}
	else {
		end = std::chrono::high_resolution_clock::now();
		counting_started = false;
	}
}

inline std::chrono::duration<double, std::ratio<1, 1>> C_Time_Counter::measured_timespan() noexcept {
	return std::chrono::duration<double, std::ratio<1, 1>>(end - beg);
}

C_Time_Counter tc;

using namespace boost::multiprecision;
using int_type = uint1024_t;

double a = 0.;
std::vector<double> sqrt_list;
size_t length_of_sqrt_list = 0;
std::vector<int_type> factorials, powers_of_2;

int_type gen_and_check(double sequence_sqrt_sum, size_t sequence_length, size_t sqrt_list_index) {
	int_type possible_sequences_permutations_sum_local = 0;
	auto places_to_insert_new_sqrt = sequence_length + 1;
	auto new_sum = a - sequence_sqrt_sum;
	for (auto sqrt_to_insert_index = sqrt_list_index; sqrt_to_insert_index < length_of_sqrt_list - 1; sqrt_to_insert_index++) {
		auto sqrt_to_insert = sqrt_list[sqrt_to_insert_index];
		auto max_possible_sqrts_inserted = static_cast<size_t>(std::floor(new_sum / sqrt_to_insert));
		for (size_t number_of_sqrts_inserted = 1; number_of_sqrts_inserted <= max_possible_sqrts_inserted; number_of_sqrts_inserted++) {
			int_type possible_ways_to_insert = factorials[number_of_sqrts_inserted + places_to_insert_new_sqrt - 1] / (factorials[number_of_sqrts_inserted] * factorials[places_to_insert_new_sqrt - 1]);
			auto& inserted_elements_sign_variations = powers_of_2[number_of_sqrts_inserted];
			auto possible_signed_ways_to_insert = possible_ways_to_insert * inserted_elements_sign_variations;
			auto new_sqrt_list_index = sqrt_to_insert_index + 1;
			auto new_sequence_length = sequence_length + number_of_sqrts_inserted;
			auto new_sequence_sqrt_sum = std::fma(number_of_sqrts_inserted, sqrt_to_insert, sequence_sqrt_sum);
			possible_sequences_permutations_sum_local += possible_signed_ways_to_insert * (gen_and_check(new_sequence_sqrt_sum, new_sequence_length, new_sqrt_list_index) + 1);
		}
	}
	{ //ostatnie przejście pętli dla sqrt_to_insert_index==length_of_sqrt_list-1 żeby uniknąć instrukcji warunkowej
		auto sqrt_to_insert_index = length_of_sqrt_list - 1;
		auto sqrt_to_insert = sqrt_list[sqrt_to_insert_index];
		auto max_possible_sqrts_inserted = static_cast<size_t>(std::floor(new_sum / sqrt_to_insert));
		for (size_t number_of_sqrts_inserted = 1; number_of_sqrts_inserted <= max_possible_sqrts_inserted; number_of_sqrts_inserted++) {
			int_type possible_ways_to_insert = factorials[number_of_sqrts_inserted + places_to_insert_new_sqrt - 1] / (factorials[number_of_sqrts_inserted] * factorials[places_to_insert_new_sqrt - 1]);
			auto& inserted_elements_sign_variations = powers_of_2[number_of_sqrts_inserted];
			auto possible_signed_ways_to_insert = possible_ways_to_insert * inserted_elements_sign_variations;
			possible_sequences_permutations_sum_local += possible_signed_ways_to_insert;
		}
	}
	return possible_sequences_permutations_sum_local;
}

int_type _run_(double a_arg) {
	a = a_arg;
	sqrt_list.clear();
	auto n = static_cast<int>(std::ceil(std::sqrt(1 + 4 * a * a) + 1));
	for (int i = 1; i < n; i++) {
		sqrt_list.push_back(sqrt(i / 2. * (i / 2. + 1)));
	}
	length_of_sqrt_list = sqrt_list.size();
	return gen_and_check(0, 0, 0);
}

void sq_test() {
	powers_of_2.clear();
	factorials.clear();
	int_type power_2 = 1;
	for (auto i = 0; i < 500; i++) {
		powers_of_2.push_back(power_2);
		power_2 *= 2;
	}
	factorials.push_back(1);
	int_type factorial = 1;
	for (int mtpl = 1; mtpl < 200; mtpl++) {
		factorial *= mtpl;
		factorials.push_back(factorial);
	}
	for (auto i = 74; i < 200; i++) {
		tc.start();
		const auto res = _run_(i);
		tc.stop();
		std::cout << i << "	" << res << "	" << tc.measured_timespan().count() << '\n';
	}
}

int main() {
	sq_test();
	return 0;
}
#ifdef x_w_text_param_H_defined
#ifdef x_w_text_param_literals_H_defined

inline start_hotspot operator""_hotspot(const char *s, size_t n)
{
	return start_hotspot{ std::string_view{s, n} };
}

inline start_hotspot operator""_hotspot(unsigned long long n)
{
	return start_hotspot{static_cast<int>(n)};
}
#endif
#endif

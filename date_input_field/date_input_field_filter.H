//
// Unit-tested logic to filter input into a date input field.
//
// #included into the date_input_field's on_filter() callback.
//
// References:
//
// date_format - a std::u32string formatted like "mm/dd/YYY", or "YYYY.mm.dd".
// The 'm', 'd', and 'Y' char32s get interpreted as the corresponding values,
// everything else is automatic punctuation.
//
// CURRENT_INPUT_FIELD_CONTENTS() - a macro that returns a std::u32string with
// the existing contents of the input field.
//
// size - what CURRENT_INPUT_FIELD_CONTENTS().size() returns.
//
// starting_pos, n_delete, str - which part of the input field is being
// modified: starting position, number of characters to be deleted, new
// std::u32string content. starting_pos, and n_delete are mutable copies of
// the const values that get passed into the on_filter callback.
//
// This logic modifies starting_pos and n_delete, and also declares a
// new_string std::u32string, which comprises the filtered input for the
// date input field, at the end of the code. A 'return' is executed if the
// given filtered input gets completely rejected.

// Begin by expanding any areas that are going to be deleted to include the
// bounding punctuation. So, deleting the first digit of the month, day, or
// year will also delete the preceding punctuation. Ditto for the last digit.

std::u32string new_string;

{
	while (n_delete > 0 && starting_pos+n_delete < date_format.size() &&
	       starting_pos+n_delete < size)
	{
		switch (date_format[starting_pos+n_delete]) {
		case 'Y':
		case 'm':
		case 'd':
			break;
		default:
			++n_delete;
			continue;
		}
		break;
	}

	while (n_delete > 0 && starting_pos > 0 &&
	       starting_pos < date_format.size())
	{
		switch (date_format[starting_pos]) {
		case 'Y':
		case 'm':
		case 'd':
			break;
		default:
			--starting_pos;
			++n_delete;
			continue;
		}
		break;
	}

	if (starting_pos+n_delete != size)
		return;

	new_string.reserve(date_format.size());

	// Now, process the input, a character at a time.

	auto b=str.begin(), e=str.end();

	for (;;)
	{
		// What's here?

		auto format_pos=new_string.size()+starting_pos;
		if (format_pos >= date_format.size())
			break;

		auto component=date_format[format_pos];

		bool digit=false;

		// If the character being added here is a digit
		// and this part of the date string should also
		// have a digit, then we're cool. That was easy.

		if (b != e)
		{
			if (unicode_isdigit(*b))
			{
				switch (component) {
				case 'Y':
				case 'm':
				case 'd':
					new_string += *b;
					++b;
					continue;
				}
				digit=true;
			}
			else
			{
				if (format_pos > 0)
					switch (date_format[format_pos-1]) {
					case 'Y':
					case 'm':
					case 'd':
						// We have a digit before this
						// new non-digit entry.
						break;
					default:
						// Entering a non-digit after
						// we already have a non-digit
						// entry before us.
						++b;
						continue;
					}
			}
		}

		// Two possibilities. A punctuation was typed
		// where a digit goes. I.e. enter a one digit
		// month, and hit slash or period. Need to
		// zero-fill the month or the day.
		//
		// The second possibility is that a separator
		// was typed. Given how this logic
		// automatically adds separators this should
		// simply be ignored, however if something
		// gets pasted we need to explicitly skip
		// over the punctuation in the new text.
		//
		// Note that we're here even when b == e, so after
		// the 2nd month or day digit gets typed, or the 4th
		// digit year, we'll end up automatically appending
		// the punctuation.

		switch (component) {
		case 'Y':
		case 'm':
		case 'd':
			break;
		default:

			new_string += component;

			// And let's advance the input being processed until
			// the next new digit.
			while (b != e && !unicode_isdigit(*b))
				++b;
			continue;
		}

		if (b == e)
			break;

		if (!digit)
			// If this was a digit, we'll reprocess it above.
			++b;

		// Let's figure out the story so far. Existing
		// contents, plus any new text.
		//
		// We already have this recursive mutex lock.

		auto in_progress=CURRENT_INPUT_FIELD_CONTENTS()
			.substr(0, starting_pos)+new_string;

		// Obtain the numeric value that was entered so far.
		//
		// Peel off each trailing digit in in_progress, until we
		// reach the start of the entered numeric value.

		uint16_t v=0;
		uint16_t m=1;

		char32_t uc;

		while (!in_progress.empty() &&
		       unicode_isdigit(uc=*--in_progress.end()))
		{
			v += m * (uc & 0x0f);
			m *= 10;
			in_progress=in_progress.substr(0,in_progress.size()-1);
		}

		// If the entered value began BEFORE the new text, we will
		// adjust starting_pos and n_deleted accordingly.

		if (in_progress.size() < starting_pos)
		{
			auto backup=starting_pos-in_progress.size();

			new_string.clear();
			starting_pos -= backup;
			n_delete += backup;
		}
		else
		{
			// Otherwise, back up new_string until the start of
			// the new value.
			new_string=in_progress.substr(starting_pos);
		}

		// If this is not a year, we must have a two-digit value for
		// the month or the day. Assume that.

		if (component != 'Y')
		{
			new_string += '0';
			new_string += '0'+v;
			continue;
		}

		// Figure out what the year this is, this must be a two-digit
		// year.

		std::ostringstream o;

		bool insert=true;
		for (char c:date_format)
		{
			switch (c) {
			case 'Y':
			case 'm':
			case 'd':
				if (insert)
				{
					if (c == 'Y')
						o << v;
					else
						o << "01";
				}
				insert=false;
				break;
			default:
				o << c;
				insert=true;
			}
		}

		auto year=ymd::parser().parse(o.str()).get_year();

		std::string y=tostring(year);
		new_string += std::u32string{y.begin(), y.end()};
	}
}

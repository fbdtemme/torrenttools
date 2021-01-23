# termcontrol

[![build](https://github.com/fbdtemme/bencode/workflows/build/badge.svg?branch=master)](https://github.com/fbdtemme/termcontrol/actions?query=workflow%3Abuild)

A small C++20 library for ANSI control sequences and terminal bits.

## Examples

All examples assume `namespace tc = termcontrol` for brevity.

Generate or print escape codes.

```{cpp}
// Generate a control sequence as std::string
std::string erase_page_after = tc::format<tc::def::erase_in_page>(tc::erase_in_page_mode::after);

// Directly print a control sequence to an output stream or output iterator.
tc::format_to<tc::def::erase_in_page>(std::cout, tc::erase_in_page_mode::after);

// Parameters that are specific to a control sequence can be printed without needing to
// specify the control_sequence_definition by using operator<<
std::cout << tc::erase_in_page_mode::after;

// Make cursor invisible
std::cout << tc::def::reset_mode(tc::dec_mode::cursor_visible);
```

Create compile time control sequences.

```
constexpr auto move_up_10 = tc::control_sequence<tc::def::cursor_up>(10);
constexpr auto erase_page_after = tc::control_sequence<tc::def::erase_in_page>(tc::erase_in_page_mode::after);
```

Format styled text.

```
// underlined with red foregroung
text_style style = em(tc::emphasis::underline) | fg(tc::terminal_color::red);
std::cout << style << "ANSI" << text_style::reset();

// equivalent command without temporaries.
tc::format_to(std::cout, em(tc::emphasis::underline) | fg(tc::terminal_color::red, "Test");
```

Colors can be specified with predefined color schemes `css_color`,
`x11_color` and `pwc_color`, or as `rgb` values.

```
auto style1 = fg(tc::css_color::lightslategray);
auto style2 = fg(tc::x11_color::light_goldenrod_yellow);
auto style3 = fg(tc::pwc_color::light_brown);
auto style4 = fg(tc::rgb(250,250,250))
```

const std: type = @import("std");

// Tokenizer stuff:

const tokentype: type = enum {
    DOTDOT, // :
    COMMA, // ,
    EQUALS, // =

    O_BRACE, // {
    C_BRACE, // }

    VALUE, // "value"
    IDENTIF, // identif

    EOF, // eof
};

const token: type = struct {
    value: []const u8,
    type: tokentype,
    x: usize,
    y: usize,
};

const tokens: type = struct {
    tokens: []token,
    number: usize,
};

fn tokenizer(src: []const u8) !tokens {
    var t: std.ArrayList(token) = std.ArrayList(token).init(std.heap.page_allocator);
    defer t.deinit();
    var x: usize = 1;
    var y: usize = 1;
    var pos: usize = 0;

    var num: usize = 0;

    while (pos <= src.len) {
        switch (src[pos]) {
            '\n' => {
                y += 1;
            },
            '#' => {
                while (src[pos] != '\n') {
                    if (pos == src.len) {
                        break;
                    }
                    pos += 1;
                    x += 1;
                }
                y += 1;
            },
            '=' => {
                t.append(token{
                    .type = tokentype.EQUALS,
                    .value = "=",
                    .x = x,
                    .y = y,
                });
                num += 1;
                x += 1;
                pos += 1;
            },
            '{' => {
                t.append(token{
                    .type = tokentype.O_BRACE,
                    .value = "{",
                    .x = x,
                    .y = y,
                });
                num += 1;
                x += 1;
                pos += 1;
            },
            '}' => {
                t.append(token{
                    .type = tokentype.C_BRACE,
                    .value = "}",
                    .x = x,
                    .y = y,
                });
                num += 1;
                x += 1;
                pos += 1;
            },
            ':' => {
                t.append(token{
                    .type = tokentype.DOTDOT,
                    .value = ":",
                    .x = x,
                    .y = y,
                });
                num += 1;
                x += 1;
                pos += 1;
            },
            '"' => {
                pos += 1;
                x += 1; // skip first "
                var str: std.ArrayList(u8) = std.ArrayList(u8).init(std.heap.page_allocator);
                defer str.deinit();
                while (true) {
                    if (pos >= src.len) {
                        std.debug.panicExtra(null, "ERROR (Tokenizer): String ended before closing quote at line {d} column {d}.", .{ y, x });
                    }
                    if (pos + 1 < src.len) {
                        if (src[pos + 1] == '"' and src[pos] == '\\') {
                            str.append("\"");
                            pos += 2;
                            x += 2;
                        }
                    }
                    if (src[pos] == '"') { // skip past the last "
                        pos += 1;
                        x += 1;
                        break;
                    }
                    str.append(src[pos]);
                    pos += 1;
                    x += 1;
                }

                t.append(token{
                    .type = tokentype.VALUE,
                    .value = @as([]const u8, str.items),
                    .x = x,
                    .y = y,
                });
                num += 1;
            },
            else => {
                var str: std.ArrayList(u8) = std.ArrayList(u8).init(std.heap.page_allocator);
                defer str.deinit();
                while (src[pos] != ' ' or src[pos] != '\t' or src[pos] == '{' or src[pos] == '}' or src[pos] == '=' or src[pos] == ':' or src[pos] == '"') {
                    str.append(src[pos]);
                    pos += 1;
                    x += 1;
                }
                t.append(token{
                    .type = tokentype.IDENTIF,
                    .value = @as([]const u8, str.items),
                    .x = x,
                    .y = y,
                });
                num += 1;
            },
        }
    }
    t.append(token{
        .type = tokentype.EOF,
        .value = "eof",
        .x = x,
        .y = y,
    });
    num += 1;

    return tokens{ .number = t.capacity, .tokens = t.items };
}

// Parser stuff

// Config

const repo: type = struct {
    name: []const u8,
    link: []const u8,
};

pub const config: type = struct {
    repos: []repo,
};

pub fn parse_config(src: []const u8) !config {
    const toks: tokens = tokenizer(src);

    var i: usize = 0;
    while (true) {
        if (toks.tokens[i].type == tokentype.IDENTIF) {
            if (i + 2 > toks.number) {
                std.debug.panicExtra(null, "ERROR (Config parser): Got a identifier but no further statment at line {d} column {d}", .{ toks.tokens[i].y, toks.tokens[i].x });
            }
            if (toks.tokens[i + 1].type == tokentype.O_BRACE) {}
        }
    }
}

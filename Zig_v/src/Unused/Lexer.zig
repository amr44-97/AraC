const Lexer = @This();

const std = @import("std");
const utf = std.unicode;
const Out = std.io.getStdOut().writer();

text: []const u8,
allocator: std.mem.Allocator,
textIndex: usize = 0,

const ArrayList = std.ArrayList;

pub fn toUtf(allocator: std.mem.Allocator, buffer: []const u8) !ArrayList(u21) {
    var len = try utf.utf8CountCodepoints(buffer);
    var list = try ArrayList(u21).initCapacity(allocator, len);

    const TextUtfView = try utf.Utf8View.init(buffer);
    var Iter = TextUtfView.iterator();

    var x: usize = 0;
    while (x < len) : (x += 1) {
        var cdps = Iter.nextCodepointSlice().?;
        var char = try utf.utf8Decode(cdps);
        try list.append(char);
    }
    return list;
}

const TokenType = enum {
    Bad_tok,
    Eof_tok,
    Plus_tok,
    Minus_tok,
    Slash_tok,
    Star_tok,
};
const Token = struct {
    Type: TokenType,
    Intval: ?u64,
    position: usize,
};

pub const TokenIndex = usize;
pub const TokenIterator = struct {
    //var TextArray: ArrayList(u21) = try toUtf(allocator, buffer);
    TokList: []const Token,
    pos: TokenIndex = 0,
};

pub fn next(self: *Lexer) Token {
    var TextArray: ArrayList(u21) = try toUtf(self.allocator, self.text);

    var netToken = Token{
        .Type = .Eof_tok,
        .Intval = null,
        .position = self.textIndex,
    };
    var x: usize = 0;
    while (x < TextArray.items.len) : (x += 1) {
        var char: u21 = TextArray.items[x];

        switch (char) {
            '+' => {
                netToken = Token{ .Type = .Plus_tok, .Intval = null, .position = x };
            },
            '-' => {
                netToken = Token{ .Type = .Minus_tok, .Intval = null, .position = x };
            },
            '/' => {
                netToken = Token{ .Type = .Slash_tok, .Intval = null, .position = x };
            },
            '*' => {
                netToken = Token{ .Type = .Star_tok, .Intval = null, .position = x };
            },
            ' ' => {
                continue;
            },
            else => {
                if (std.ascii.isDigit(@intCast(u8, char))) {
                    try Out.print("Is integer\n", .{});
                }
            },
        }
    }
    return netToken;
}

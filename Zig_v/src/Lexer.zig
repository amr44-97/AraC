const std = @import("std");
const ArrayList = std.ArrayList;
const utf = std.unicode;
const Out = std.io.getStdOut().writer();
const fmt = std.fmt;
const mem = std.mem;

const Lexer = @This();

allocator: std.mem.Allocator,
buffer: []const u8,
UtfList: ArrayList(u21),
u8List: ArrayList([]const u8),
TextUtfView: utf.Utf8View = undefined,
TokenList: ArrayList(Token) = undefined,

var Iter: utf.Utf8Iterator = undefined;

var buf_index: usize = 0;

pub const TokenType = enum {
    Bad_Tok, // ?
    Eof_Tok, // eof
    WhiteSpace_Tok, // " "
    Equal_Tok, // =
    Equal_Equal_Tok, // ==
    Question_Mark_Tok, // ?
    Bang_Tok, // !
    AT_Tok, // @
    Pipe_Tok, // |
    Pipe_Pipe_Tok, // ||
    Not_Eql_Tok, // !=

    AND_Tok, // &
    AND_Equal_Tok, // &=
    AND_AND_Tok, // &&
    Perc_Tok, // %
    Perc_Equal_Tok, // %=
    Almost_Tok, // ~
    Almost_Equal_Tok, // ~=
    Hash_Tok, // #

    Plus_Tok, //  +
    Plus_Plus_Tok, //  ++
    Plus_Equal_Tok, //  +=
    Minus_Tok, // -
    Minus_Minus_Tok, // --
    Minus_Equal_Tok, // -=
    Slash_Tok, //  /
    Slash_Equal_Tok, //  /=
    BackSlash_Tok, // \
    Star_Tok, // *
    Star_Equal_Tok, // *=
    OpenParenthesis_Tok, // (
    CloseParenthesis_Tok, // )
    OpenCurlyBracket_Tok, // [
    CloseCurlyBracket_Tok, // ]

    Open_Brace_Tok, // {
    Close_Brace_Tok, // }

    newline_Tok, // \n
    Integer_Tok, //  123..

    Colon_Colon_Tok, // :
    Colon_Equal_Tok, // :
    Colon_Tok, // ::
    Dot_Tok, // .
    Comma_Tok, // ,

    Right_Arrow_Tok, // >
    Left_Arrow_Tok, // <
    Big_Or_Eql_Tok, // > =
    Small_Or_Eql_Tok, // < =

    Right_double_Arrow_Tok, // >>
    Left_double_Arrow_Tok, // <<

    Single_quote_Tok, // '
    Double_quote_Tok, // "

    Double_forward_Slash_Tok, // "//"
    Double_Back_Slash_Tok, //  \\
    String_Tok, // "Hello        مرحبا"
};
const Text = union {
    text: []const u8,
    string: []const u21,
};

pub const Token = struct {
    Type: TokenType,
    Intval: ?u64 = null,
    position: usize = 0,
    buffer: ArrayList(u21),
    text: Text,

    pub fn makeTok(self: *Token, Text_t: Text, Tok_Type: TokenType, intval: ?u64, pos: usize) void {
        self.Type = Tok_Type;
        self.text = Text_t;
        self.Intval = intval;
        self.position = pos;
    }
};

pub const ArrComb = struct { UtfList: ArrayList(u21), U8List: ArrayList([]const u8) };

pub fn toUtf(allocator: std.mem.Allocator, buffer: []const u8) !ArrComb {
    var len = try utf.utf8CountCodepoints(buffer);

    var List = ArrComb{ .UtfList = try ArrayList(u21).initCapacity(allocator, len), .U8List = try ArrayList([]const u8).initCapacity(allocator, buffer.len) };

    const TextUtfView = try utf.Utf8View.init(buffer);
    var Iterer = TextUtfView.iterator();

    var x: usize = 0;
    while (x < len) : (x += 1) {
        var cdps = Iterer.nextCodepointSlice().?;
        var char = try utf.utf8Decode(cdps);

        try List.UtfList.append(char);
        try List.U8List.append(cdps);
    }
    return List;
}

pub fn init(Alloc: std.mem.Allocator, buf: []const u8) !Lexer {
    var tmp = try toUtf(Alloc, buf);
    var stm = Lexer{
        .allocator = Alloc,
        .buffer = buf,
        .UtfList = tmp.UtfList,
        .u8List = tmp.U8List,
        .TextUtfView = try utf.Utf8View.init(buf),
    };
    stm.TokenList = try stm.scan();
    return stm;
}

pub fn deinit(self: *Lexer) void {
    self.u8List.deinit();
    self.UtfList.deinit();

    //try Out.print("cap = {}\n", .{self.TokenList.capacity});
    self.TokenList.deinit();
}

pub fn printTokens(self: *Lexer) !void {
    for (self.TokenList.items) |tok| {
        if (tok.Type == TokenType.Integer_Tok) {
            try Out.print("{} -- {} -- [{d}]\n", .{ tok.Type, tok.position, tok.Intval.? });
        } else if (tok.Type == TokenType.String_Tok) {
            try Out.print("{} -- {} -- [{u}]\n", .{ tok.Type, tok.position, tok.text.string });
            if (try utf.utf8CountCodepoints("أنا عمرو") == tok.text.string.len) {
                try Out.print("All Are Equal\n", .{});
            }
        } else {
            try Out.print("{} -- {} -- [{s}]\n", .{ tok.Type, tok.position, tok.text.text });
        }
    }
}

pub fn printBadTokens(self: *Lexer) !void {
    for (self.TokenList.items) |tok| {
        if (tok.Type == TokenType.Bad_Tok) {
            if (tok.Type == TokenType.Integer_Tok) {
                try Out.print("{} -- {} -- [{d}]\n", .{ tok.Type, tok.position, tok.Intval.? });
            } else if (tok.Type == TokenType.String_Tok) {
                try Out.print("{} -- {} -- [{u}]\n", .{ tok.Type, tok.position, tok.text.string });
                if (try utf.utf8CountCodepoints("أنا عمرو") == tok.text.string.len) {
                    try Out.print("All Are Equal\n", .{});
                }
            } else {
                try Out.print("{} -- {} -- [{s}]\n", .{ tok.Type, tok.position, tok.text.text });
            }
        }
    }
}

pub fn scan(self: *Lexer) !ArrayList(Token) {
    var Toklist = ArrayList(Token).init(self.allocator);
    while (true) {
        var token = try self.nextToken();
        try Toklist.append(token);
        if (token.Type == Lexer.TokenType.Eof_Tok) {
            buf_index = 0;
            break;
        }
    }
    return Toklist;
}

var tmplen: usize = 0;
var start: usize = 0;

pub fn peek(self: *Lexer, offset: usize) u21 {
    const len = self.UtfList.items.len;
    if ((buf_index + offset) >= len) {
        return 0;
    } else {
        return self.UtfList.items[buf_index + 1];
    }
}

pub fn next(self: *Lexer, offset: usize) u21 {
    const len = self.UtfList.items.len;
    if ((buf_index + offset) >= len) {
        return 0;
    } else {
        buf_index += offset;
        return self.UtfList.items[buf_index];
    }
}

pub fn nextToken(self: *Lexer) !Token {
    var buf: ArrayList(u21) = self.UtfList; //try toUtf(self.allocator, self.buffer);
    var text: ArrayList([]const u8) = self.u8List; //try toUtf(self.allocator, self.ubuffer);
    const len = buf.items.len;
    var token: Token = undefined;

    if (buf_index >= len) {
        token.makeTok(Text{ .text = "Eof" }, TokenType.Eof_Tok, null, buf_index);
        buf_index = 0;
        return token;
    }
    var char: u21 = buf.items[buf_index];
    var text_buf: []const u8 = text.items[buf_index];

    switch (char) {
        ' ' => {
            token.makeTok(Text{ .text = " " }, TokenType.WhiteSpace_Tok, null, buf_index);
        },
        '+' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "+=" }, TokenType.Plus_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                '+' => {
                    token.makeTok(Text{ .text = "++" }, TokenType.Plus_Plus_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => {
                    token.makeTok(Text{ .text = text_buf }, TokenType.Plus_Tok, null, buf_index);
                },
            }
        },
        '-' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "-=" }, TokenType.Minus_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                '-' => {
                    token.makeTok(Text{ .text = "--" }, TokenType.Minus_Minus_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Minus_Tok, null, buf_index),
            }
        },

        '=' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "==" }, TokenType.Equal_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Equal_Tok, null, buf_index),
            }
        },

        '/' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "/=" }, TokenType.Slash_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                '/' => {
                    token.makeTok(Text{ .text = "//" }, TokenType.Double_forward_Slash_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Slash_Tok, null, buf_index),
            }
        },
        '\\' => {
            switch (peek(self, 1)) {
                '\\' => {
                    token.makeTok(Text{ .text = "\\\\" }, TokenType.Double_Back_Slash_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.BackSlash_Tok, null, buf_index),
            }
        },

        '*' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "*=" }, TokenType.Star_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Star_Tok, null, buf_index),
            }
        },

        ':' => {
            switch (peek(self, 1)) {
                ':' => {
                    token.makeTok(Text{ .text = "::" }, TokenType.Colon_Colon_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                '=' => {
                    token.makeTok(Text{ .text = ":=" }, TokenType.Colon_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Colon_Tok, null, buf_index),
            }
        },

        '&' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "&=" }, TokenType.AND_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                '&' => {
                    token.makeTok(Text{ .text = "&&" }, TokenType.AND_AND_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.AND_Tok, null, buf_index),
            }
        },
        '%' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "%=" }, TokenType.Perc_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = text_buf }, TokenType.Perc_Tok, null, buf_index),
            }
        },
        '~' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "~=" }, TokenType.Almost_Equal_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = "~" }, TokenType.Almost_Tok, null, buf_index),
            }
        },
        '?' => {
            token.makeTok(Text{ .text = "?" }, TokenType.Question_Mark_Tok, null, buf_index);
        },
        '@' => {
            token.makeTok(Text{ .text = "@" }, TokenType.AT_Tok, null, buf_index);
        },
        '#' => {
            token.makeTok(Text{ .text = "#" }, TokenType.Hash_Tok, null, buf_index);
        },
        '!' => {
            switch (peek(self, 1)) {
                '=' => {
                    token.makeTok(Text{ .text = "!=" }, TokenType.Not_Eql_Tok, null, buf_index);
                    _ = next(self, 1);
                },
                else => token.makeTok(Text{ .text = "!" }, TokenType.Bang_Tok, null, buf_index),
            }
        },
        '.' => {
            token.makeTok(Text{ .text = "." }, TokenType.Dot_Tok, null, buf_index);
        },
        ',' => {
            token.makeTok(Text{ .text = "," }, TokenType.Comma_Tok, null, buf_index);
        },
        '(' => {
            token.makeTok(Text{ .text = "(" }, TokenType.OpenParenthesis_Tok, null, buf_index);
        },
        ')' => {
            token.makeTok(Text{ .text = ")" }, TokenType.OpenCurlyBracket_Tok, null, buf_index);
        },
        '{' => {
            token.makeTok(Text{ .text = "{" }, TokenType.Open_Brace_Tok, null, buf_index);
        },
        '}' => {
            token.makeTok(Text{ .text = "}" }, TokenType.Close_Brace_Tok, null, buf_index);
        },

        ']' => {
            token.makeTok(Text{ .text = "]" }, TokenType.CloseCurlyBracket_Tok, null, buf_index);
        },
        '[' => {
            token.makeTok(Text{ .text = "[" }, TokenType.OpenCurlyBracket_Tok, null, buf_index);
        },

        '"' => {
            //token.makeTok("\"", TokenType.Double_quote_Tok, null, buf_index);
            char = next(self, 1);
            start = buf_index;
            while (utf.utf8ValidCodepoint(char) and char != '"') {
                char = next(self, 1);
            }

            var ints: []const u21 = buf.items[start..buf_index];
            //var tmps: [1000]u21 = undefined;
            //var intu8 = tmps[0..ints.len];
            //var x: usize = 0;

            token.makeTok(Text{ .string = ints }, TokenType.String_Tok, null, buf_index);
        },

        '\n' => {
            token.makeTok(Text{ .text = "\\n" }, TokenType.newline_Tok, null, buf_index);
        },

        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' => {
            start = buf_index;

            while (std.ascii.isDigit(@intCast(u8, char))) {
                buf_index += 1;
                char = buf.items[buf_index];
            }
            var ints: []const u21 = buf.items[start..buf_index];
            var tmps: [1000]u8 = undefined;
            var intu8 = tmps[0..ints.len];
            var x: usize = 0;

            while (x < ints.len) : (x += 1) {
                intu8[x] = @intCast(u8, ints[x]);
            }
            token.makeTok(Text{ .text = intu8 }, TokenType.Integer_Tok, try fmt.parseInt(u64, intu8, 10), buf_index);
        },
        else => {
            token.makeTok(Text{ .text = text_buf }, TokenType.Bad_Tok, null, buf_index);
        },
    }
    buf_index += 1;
    return token;
}
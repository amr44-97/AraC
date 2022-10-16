const Lexer = @import("Lexer.zig");
const std = @import("std");
const File = std.fs.File;
const mem = std.mem;
const fmt = std.fmt;
const Out = std.io;
const utf = std.unicode;
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

const Parser = @This();
pub const AstType = enum { Ast_ADD, Ast_SUBSTRACT, Ast_DIVIDE, AST_MULTIPLY, Ast_INTLIT };
pub const AstNode = struct {
    tok: Lexer.Token,
    Intval: ?u64,
    op: AstType,
    left: ?*AstNode,
    right: ?*AstNode,
    valid: bool,
};

lexer: Lexer,
allocator: std.mem.Allocator,
Tree: ?*AstNode = null,

var Current_token: Lexer.Token = undefined;

pub fn init(Alloca: mem.Allocator, buffer: []const u8) !Parser {
    var pars_er = Parser{ .lexer = try Lexer.init(Alloca, buffer), .allocator = Alloca };
    Current_token = try pars_er.lexer.nextToken();
    return pars_er;
}

pub fn freeNode(node: ?*AstNode) void {
    if (node != null) {
        gpa.allocator().destroy(node.?);
        freeNode(node.?.left);
        freeNode(node.?.right);
    }
}

pub fn deinit(self: *Parser) void {
    self.lexer.deinit();
    if (self.Tree != null) {
        gpa.allocator().destroy(self.Tree.?);
    }
}

pub fn nextToken(self: *Parser) !Lexer.Token {
    var T = try self.lexer.nextToken();
    while (T.Type == Lexer.TokenType.WhiteSpace_Tok or T.Type == Lexer.TokenType.newline_Tok) {
        T = try self.lexer.nextToken();
    }
    return T;
}

pub fn makeAstNode(tok: Lexer.Token, Intval: ?u64, op: AstType, left: ?*AstNode, right: ?*AstNode) !?*AstNode {
    var s: *AstNode = try allocator.create(AstNode);
    s.* = AstNode{ .tok = tok, .Intval = Intval, .op = op, .left = left, .right = right, .valid = true };
    return s;
}

pub fn makeAstleaf(tok: Lexer.Token, Intval: ?u64, op: AstType) !?*AstNode {
    var x = try makeAstNode(tok, Intval, op, null, null);
    return x;
}

pub fn makeAstUnary(tok: Lexer.Token, Intval: u64, op: AstType, left: ?*AstNode) !?AstNode {
    return {
        try makeAstNode(tok, Intval, op, left, null);
    };
}

pub fn tokASt(Type: Lexer.TokenType) AstType {
    var TPe: AstType = undefined;
    switch (Type) {
        Lexer.TokenType.Plus_Tok => TPe = AstType.Ast_ADD,
        Lexer.TokenType.Minus_Tok => TPe = AstType.Ast_SUBSTRACT,
        Lexer.TokenType.Slash_Tok => TPe = AstType.Ast_DIVIDE,
        Lexer.TokenType.Star_Tok => TPe = AstType.AST_MULTIPLY,
        else => {},
    }
    return TPe;
}

pub fn primaryExpression(self: *Parser) !?*AstNode {
    var node: ?*AstNode = null;
    if (Current_token.Type == Lexer.TokenType.Integer_Tok) {
        node = try makeAstleaf(Current_token, Current_token.Intval.?, AstType.Ast_INTLIT);
        Current_token = try self.nextToken();
        return node;
    }
    @panic("syntax Error at primaryExpression()!!\n");
}

pub fn getpresd(T: Lexer.TokenType) usize {
    if (T == Lexer.TokenType.Plus_Tok or T == Lexer.TokenType.Minus_Tok)
        return 1;
    if (T == Lexer.TokenType.Slash_Tok or T == Lexer.TokenType.Star_Tok)
        return 2;

    return 0;
}
pub fn parseNumExpr(self: *Parser, ptp: usize) !?*AstNode {
    var left: ?*AstNode = undefined;
    var right: ?*AstNode = undefined;

    left = try self.primaryExpression();

    var prevTok = Current_token;

    if (Current_token.Type == Lexer.TokenType.Eof_Tok)
        return left;

    while (getpresd(prevTok.Type) > ptp) {
        Current_token = try nextToken(self);

        right = try parseNumExpr(self, getpresd(prevTok.Type));

        left = try makeAstNode(prevTok, null, tokASt(prevTok.Type), left, right);

        prevTok = Current_token;

        if (prevTok.Type == Lexer.TokenType.Eof_Tok) {
            return left;
        }
    }

    return left;
}

pub fn parseExpr(self: *Parser) !*AstNode {
    self.Tree = try self.parseNumExpr(0);
    return self.Tree.?;
}

pub fn prettyprint(prefix: []const u8, Node: ?*Parser.AstNode, isLeft: bool) !void {
    if (Node != null) {
        try Out.getStdOut().writer().print("{s}", .{prefix});
        if (isLeft) {
            try Out.getStdOut().writer().print("├──", .{});
        } else {
            try Out.getStdOut().writer().print("└──", .{});
        }

        if (Node.?.op == AstType.Ast_INTLIT) {
            try Out.getStdOut().writer().print("({})\n", .{Node.?.Intval.?});
        } else {
            try Out.getStdOut().writer().print("({s})\n", .{Node.?.tok.text.text});
        }

        //       if (isLeft) {
        try prettyprint(if (isLeft) try mem.concat(allocator, u8, &[_][]const u8{ prefix, "│   " }) else try mem.concat(allocator, u8, &[_][]const u8{ prefix, "    " }), Node.?.left, true);

        try prettyprint(if (isLeft) try mem.concat(allocator, u8, &[_][]const u8{ prefix, "│   " }) else try mem.concat(allocator, u8, &[_][]const u8{ prefix, "    " }), Node.?.right, false);
    }
}

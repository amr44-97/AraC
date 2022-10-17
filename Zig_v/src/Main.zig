const std = @import("std");
const File = std.fs.File;
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();
const Parser = @import("Parser.zig");
//const Lexer = @import("Lexer.zig");
const os = std.os;

pub fn main() !void {
    var iter = try std.process.argsWithAllocator(allocator);
    const Out = std.io;
    //var iter = arg.init();

    _ = iter.skip();
    var arg = iter.next();
    if (arg == null) {
        _ = try Out.getStdOut().write("[Error]: Not Enough Arguments\n");
        return;
    }
    var fp: File = try std.fs.cwd().openFile(arg.?, .{ .mode = .read_write });
    const fp_s = try fp.getEndPos();
    var txtbuf = try fp.readToEndAlloc(allocator, fp_s);
    var Parse = try Parser.init(allocator, txtbuf);
    defer Parse.deinit();

    try Parse.lexer.printBadTokens();
    _ = try Out.getStdOut().write("-------------------------------------------------\n");
    _ = try Out.getStdOut().write("-------------------------------------------------\n");
    _ = try Out.getStdOut().write("-------------------------------------------------\n");
    try Parse.lexer.printTokens();
    //var n = try Parse.parseExpr();

    //try Parser.prettyprint("",n,false);
}

test "Alloc" {}

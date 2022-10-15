const std = @import("std");
const File = std.fs.File;
const mem = std.mem;
const fmt = std.fmt;
const Out = std.io.getStdOut().writer();
const utf = std.unicode;
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const Lexer = @import("Lexer.zig");

pub fn main() !void {
    const allocator = gpa.allocator();
    var fp: File = try std.fs.cwd().openFile("test.ara", .{ .mode = .read_write });
    const fp_s = try fp.getEndPos();
    var buf = try fp.readToEndAlloc(allocator, fp_s);

    const TextSize = try utf.utf8CountCodepoints(buf);
    const TextUtfView = try utf.Utf8View.init(buf);
    var Iter = TextUtfView.iterator();

    var x: usize = 0;
    while (x < TextSize) : (x += 1) {
        var cdps = Iter.nextCodepointSlice().?;
        var char = try utf.utf8Decode(cdps);

        switch (char) {
            '+' => {
                try Out.print("Tok(+)\n", .{});
            },
            '-' => {
                try Out.print("Tok(-)\n", .{});
            },
            '/' => {
                try Out.print("Tok(/)\n", .{});
            },
            '*' => {
                try Out.print("Tok(*)\n", .{});
            },
            else => {
                try Out.print("token = {s} : size = {}\n", .{ cdps, try utf.utf8CountCodepoints(cdps) });
                var c: u8 = @intCast(u8, char); //char >> (2 * 8);

                if (std.ascii.isDigit(c)) {
                    try Out.print("is digit\n", .{});
                }
            },
        }
    }

    var res = try Lexer.toUtf(allocator, buf);
    for (res.item) |i| {
        try Out.print("utf = {u}\n", .{i});
    }
}
test "simple test" {
    var list = std.ArrayList(i32).init(std.testing.allocator);
    defer list.deinit(); // try commenting this out and see if zig detects the memory leak!
    try list.append(42);
    try std.testing.expectEqual(@as(i32, 42), list.pop());
}

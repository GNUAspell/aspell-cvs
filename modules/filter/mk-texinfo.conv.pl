use utf8;

my %table;
my $table = <<'---';
@aa{}        U+00E5 LATIN SMALL LETTER A WITH RING ABOVE
@AA{}        U+00C5 LATIN CAPITAL LETTER A WITH RING ABOVE
@ae{}        U+00F8 LATIN SMALL LETTER O WITH STROKE
@AE{}        U+00C6 LATIN CAPITAL LETTER AE
@dotless{i}  U+0131 LATIN SMALL LETTER DOTLESS I
@dotless{j}
@l{}         U+0142 LATIN SMALL LETTER L WITH STROKE
@L{}         U+0141 LATIN CAPITAL LETTER L WITH STROKE
@o{}         U+00F8 LATIN SMALL LETTER O WITH STROKE
@O{}         U+00D8 LATIN CAPITAL LETTER O WITH STROKE
@oe{}        U+0153 LATIN SMALL LIGATURE OE
@OE{}        U+0152 LATIN CAPITAL LIGATURE OE
@ss{}        U+00DF LATIN SMALL LETTER SHARP S
---

foreach (split /\n/, $table) {
  next unless /^(\S+)\s+U\+(....)/;
  $table{hex($2)} = [$1];
}

%comb = 
(
 0x0308 => ['@"', ''],
 0x0301 => ["\@\'", ''],
 0x0327 => ['@,', '{}'],
 0x0304 => ['@=',''],
 0x0302 => ['@^',''],
 0x0300 => ['@`',''],
 0x0303 => ['@~',''],
 0x0307 => ['@dotaccent','{}'],
 0x030B => ['@H','{}'],
 0x030A => ['@ringaccent','{}'],
 0x0306 => ['@u','{}'],
 0x0331 => ['@ubaraccent','{}'],
 0x0323 => ['@udotaccent','{}'],
 0x030C => ['@v','{}'],
);

open F, "/home/kevina/devel/aspell-lang/decomp.txt";
while (<F>) {
  next unless /^(....) = (....) (....)$/;
  my ($a, $b, $c) = (hex($1), hex($2), hex($3));
  next unless exists $comb{$c};
  if ($b < 0x80) {
    push @{$table{$a}}, ($comb{$c}[0].chr($b)) if $comb{$c}[1] eq '';
    push @{$table{$a}}, ($comb{$c}[0].'{'.chr($b).'}');
  } 
  #elsif (exists $table{$b}) {
  #  push @{$table{$a}}, ($comb{$c}[0].'{'.$table{$b}[0].'}');
  #}
}

open F, ">:utf8", "texinfo.conv";

print F "\# Generated from mk-texinfo.conv.pl\n";
print F "name texinfo\n";
print F "table\n";
print F ". \@-\n";
print F ". \@/\n";

foreach (sort {$a <=> $b} keys %table)
{
  print F chr($_), ' ', join(' ', @{$table{$_}}), "\n";
}

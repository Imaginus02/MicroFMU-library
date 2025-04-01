#!/bin/bash

# Directory to search for .c files
SEARCH_DIR="library/fmu/sources"
ALLOC_FILE=".alloc"

# Clear any existing .alloc file before starting
> "$ALLOC_FILE"

# Step 1: Gather all calloc calls into a .alloc file, keeping info across files.
echo "Step 1: Gathering calloc calls into .alloc..."

find "$SEARCH_DIR" -type f -name '*.c' | while read -r file; do
    echo "Processing file: $file"
    
    # Gather all calloc calls into the .alloc file
    perl -0777 -ne '
        while (/\b(\w+)\s*=\s*(?:\(\s*\w+\s*\*\s*\)\s*)?calloc\s*\(\s*(.+?)\s*,\s*sizeof\s*\(\s*(\w+)\s*\)\s*\)/gx) {
            print "$1|$2|$3\n";
        }
    ' "$file" >> "$ALLOC_FILE"
done

# Remove duplicate entries in the .alloc file
sort -u "$ALLOC_FILE" > "${ALLOC_FILE}.tmp" && mv "${ALLOC_FILE}.tmp" "$ALLOC_FILE"

echo "Step 1 complete: calloc calls have been gathered in $ALLOC_FILE."


# Step 2: Replace calloc with m_malloc and update files
echo "Step 2: Replacing calloc with m_malloc..."

find "$SEARCH_DIR" -type f -name '*.c' | while read -r file; do
    tmpfile=$(mktemp)

    # Perform the replacement and only write back if there's a change
    perl -0777 -pe '
        my $orig = $_;
        s{
            (\**\b\w+\b)\s*=\s*(?:\(\s*\w+\s*\*\s*\)\s*)?calloc\s*\(\s*(.+?)\s*,\s*sizeof\s*\(\s*(\w+)\s*\)\s*\);
        }{
            "#ifdef MICROPY_ESP_IDF_4\n        $1 = m_malloc(($2) * sizeof($3));\n#else\n        $1 = calloc($2, sizeof($3));\n#endif"
        }egx;
        $_ eq $orig ? exit(1) : $_  # Return false if no changes made
    ' "$file" > "$tmpfile" && mv "$tmpfile" "$file" || rm "$tmpfile"
done

echo "Step 2 complete: calloc replaced with m_malloc."


# Step 3: Replace free with m_free based on .alloc file
echo "Step 3: Replacing free with m_free..."

# Create a single debug file
debug_file="free_replacement_debug.log"
echo "Free replacement debug log" > "$debug_file"

find "$SEARCH_DIR" -type f -name '*.c' | while read -r file; do
    echo "Processing $file..." >> "$debug_file"
    tmpfile=$(mktemp)

    # Load the .alloc file content into a variable for easier use in the Perl script
    alloc_content=$(cat "$ALLOC_FILE")

    perl -0777 -pe '
        # The content of the .alloc file passed from bash
        my @alloc_lines = split(/\n/, q('"$alloc_content"'));
        
        # Build our hash of allocated variables
        my %allocs;
        foreach my $line (@alloc_lines) {
            my ($var, $n, $type) = split(/\|/, $line);
            $allocs{$var} = "$n|$type";
        }
        
        # Function to clean a variable name for matching
        sub clean_var_name {
            my $name = shift;
            $name =~ s/^\s+|\s+$//g;  # Trim whitespace
            
            # Extract the core variable name from cast expressions like (void*)comp->resourceLocation
            if ($name =~ /\(\s*\w+(?:\s*\*\s*)?\)\s*(\w+(?:->|\.)[\w->\.]+)/) {
                return $1;
            }
            
            # Extract the core variable from expressions like (void*)var
            if ($name =~ /\(\s*\w+(?:\s*\*\s*)?\)\s*(\w+)/) {
                return $1;
            }
            
            $name =~ s/^\&//;         # Remove address-of operator
            $name =~ s/^\*+//;        # Remove pointer dereference
            
            
            $name =~ s/\[\d+\].*$//;  # Remove array subscript
            return $name;
        }
        
        # Replace free calls, being careful to match variable names from .alloc file
		s{
			(.*\b)free\s*\(\s*(.*?)\s*\);
        }{
			my $bfr = $1;
            my $ptr = $2;
            my $base = clean_var_name($ptr);
            
            open(my $debug, ">>", "'"$debug_file"'");
            print $debug "  Found: free($ptr);\n";
            print $debug "  Cleaned to: $base\n";
            
            my $result;
            if (exists $allocs{$base}) {
                my ($n, $type) = split(/\|/, $allocs{$base});
                print $debug "  MATCHED with: $base -> $n, $type\n";
                #$result = "#ifdef MICROPY_ESP_IDF_4\n        m_free($ptr);\n#else\n        m_free($ptr, ($n) * sizeof($type));\n#endif";
				if ($bfr =~ /.*\bif\b.*/) {
					$result = "$bfr\n{\n#ifdef MICROPY_ESP_IDF_4\n        m_free($ptr);\n#else\n        free($ptr);\n#endif\n}";
                } else {
                    $result = "#ifdef MICROPY_ESP_IDF_4\n        m_free($ptr);\n#else\n        free($ptr);\n#endif";
                }
            } else {
                print $debug "  NOT MATCHED: Simple replacement\n";
                if ($bfr =~ /.*\bif\b.*/) {
                    $result = "$bfr\n{\n#ifdef MICROPY_ESP_IDF_4\n        m_free($ptr);\n#else\n        free($ptr);\n#endif\n}";
                } else {
                    $result = "#ifdef MICROPY_ESP_IDF_4\n        m_free($ptr);\n#else\n        free($ptr);\n#endif";
                }
            }
            close($debug);
            $result;
        }egx;
    ' "$file" > "$tmpfile"
    
    # Always apply changes for now to ensure we're making progress
    mv "$tmpfile" "$file"
done

echo "Step 3 complete: free replaced with m_free."

# Step 4: Replace realloc with m_realloc
echo "Step 4: Replacing realloc with m_realloc..."

find "$SEARCH_DIR" -type f -name '*.c' | while read -r file; do
    tmpfile=$(mktemp)
    
    # Perform the replacement and only write back if there's a change
    perl -0777 -pe '
		my $orig = $_;
		s{
			(.*\s*)\brealloc\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\);
		}{
			"#ifdef MICROPY_ESP_IDF_4\n        ${1}m_realloc($2, $3);\n#else\n        ${1}m_realloc($2, $3, $3);\n#endif"
		}egx;
        $_ eq $orig ? exit(1) : $_  # Return false if no changes made
    ' "$file" > "$tmpfile" && mv "$tmpfile" "$file" || rm "$tmpfile"
done

echo "Step 4 complete: realloc replaced with m_realloc."


# Step 5: Add #include "py/obj.h" after #include "cosimulation.h"
echo "Step 5: Adding #include \"py/obj.h\" after #include \"cosimulation.h\"..."

find "$SEARCH_DIR" -type f -name '*.c' | while read -r file; do
    tmpfile=$(mktemp)
    
    # Perform the insertion and only write back if there's a change
    perl -0777 -pe '
        my $orig = $_;
        s{
            (\#include\s+"cosimulation\.h")
        }{
            "$1\n#include \"py/obj.h\""
        }egx;
        $_ eq $orig ? exit(1) : $_  # Return false if no changes made
    ' "$file" > "$tmpfile" && mv "$tmpfile" "$file" || rm "$tmpfile"
done

echo "Step 5 complete: #include \"py/obj.h\" added."

# Final summary
echo "Script execution complete."
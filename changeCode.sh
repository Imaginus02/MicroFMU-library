#!/bin/bash

# Directory to search for .c files
SEARCH_DIR="library/fmu/sources"

echo "Starting code changes...\nStep 1: Add include ufmu_alloc.h to all concerned files"

grep -l 'alloc' library/fmu/sources/* | xargs sed -i '1s|^|#include "../../ufmu_alloc.h"\n|'

echo "Step 1 complete."



echo "Step 2: Replacing calloc with m_malloc..."

sed -i "s/calloc/ufmu_malloc/g" ./library/fmu/sources/*

echo "Step 2 complete."


echo "Step 3: Replacing free with ufmu_free..."

sed -i "s/free/ufmu_free/g" ./library/fmu/sources/*

echo "Step 3 complete: free replaced with m_free."

# Step 4: Replace realloc with m_realloc
echo "Step 4: Replacing realloc with m_realloc..."

grep -l 'realloc' library/fmu/sources/* | xargs sed -i '1s|^|#include "py/obj.h"\n|'

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

echo "Step 4 complete: realloc replaced with m_realloc.\nScript execution complete."
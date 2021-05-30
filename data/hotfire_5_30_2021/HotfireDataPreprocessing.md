# DAQ Saved Code Pre-processing

When collecting data through Prometheus' DAQ system, a few errors from serial transmission were also transmitted to the MATALB GUI, which performed data saving. Also, a good portion of data was saved incorrectly/in bad format. We perform some preprocessing as follows:

```bash
# to type ^M (Windows carriage return), type CTRL+V, CTRL+M
sed -e "s/^M//" raw_data_filename | grep -Eo '^[0-9]{7},([-]?[0-9]+[.][0-9]{2},){4}\-42069' >output_filename
```

This preprocessing ensures all data lines are of the correct format (CSV format with 6 fields that are correctly formatted/containing values we expect).
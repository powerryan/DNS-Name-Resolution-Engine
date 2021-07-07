### Ryan Power

Multi-threaded application that resolves domain names to IP addresses.

## Description  
The file names specified by `<data file>` are passed to the pool of parser
threads which place information into a shared data area. Converter
threads read the shared data area and find the corresponding IP address.

### multi-lookup  
resolves a set of domain names to IP addresses

### Command line Arguments
number of parsing threads, number of conversion threads, parsing log, converter log, [input data files]

### Build  
Enter into the command line:  
`make`

### Run  
Enter into the command line:  
`./multi-lookup <# parsing threads> <# conversion threads>
<parsing log> <converter log> [ <datafile> ...]`

# Command line arguments

Command line arguments can be used to control certain functions of the OpenCL hardware capability viewer. You can e.g. upload reports without having to invoke the UI, enable logging to a text file, etc. Most of these arguments are aimed at the command line build of the tool, but they're also available with the gui build.

| Argument | Description | Example |
| - | - | - |
| --help | Print all available command line arguments | |
| --log | Write log messages to a text file for debugging (log.txt) | |
| --devices | List available OpenCL devices with their device indices | |
| --save | <savereport> Save report to disk | --save opencl_report.json |
| --upload | Upload report for device with given index to the database without visual interaction | |
| --deviceindex | Set optional zero based device index for report upload, if not set, the first device will be used | --deviceindex 2 |
| --submitter <submitter> | Set optional submitter name for report upload | --submitter "Some person" |
| --comment <comment> | Set optional comment for report upload | --comment "Beta driver" |
| --noproxy | Disable proxy settings (if specified in the settings file) | |

If you e.g. want to upload a report for the second OpenCL device in the list displayed by `--devices` along with a submitter name and comment you'd do something like this:

```bash
./OpenCLCapsViewer --upload --deviceindex 1 --submitter "My name" --comment "Beta driver"
```


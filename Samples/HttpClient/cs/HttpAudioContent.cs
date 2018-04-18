//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.Web.Http;
using Windows.Web.Http.Headers;

namespace SDKTemplate
{
    class HttpAudioContent : IHttpContent
    {
        HttpContentHeaderCollection headers;

        public HttpContentHeaderCollection Headers
        {
            get
            {
                return headers;
            }
        }

        public HttpAudioContent()
        {
            headers = new HttpContentHeaderCollection();
            headers.ContentType = new HttpMediaTypeHeaderValue("application/octet-stream");
            headers.ContentType.CharSet = "UTF-8";
        }

        public IAsyncOperationWithProgress<ulong, ulong> BufferAllAsync()
        {
            return AsyncInfo.Run<ulong, ulong>((cancellationToken, progress) =>
            {
                return Task<ulong>.Run(() =>
                {
                    ulong length = GetLength();

                    // Report progress.
                    progress.Report(length);

                    // Just return the size in bytes.
                    return length;
                });
            });
        }

        public IAsyncOperationWithProgress<IBuffer, ulong> ReadAsBufferAsync()
        {
            return AsyncInfo.Run<IBuffer, ulong>((cancellationToken, progress) =>
            {
                int count = 360;
                return Task<IBuffer>.Run(() =>
                {
                    byte[] data = new byte[count];
                    for (uint i = 0; i < count; i++)
                    {
                        data[i] = 64;
                    }

                    progress.Report(0);
                    return data.AsBuffer();
                });
            });
        }

        public IAsyncOperationWithProgress<IInputStream, ulong> ReadAsInputStreamAsync()
        {
            return AsyncInfo.Run<IInputStream, ulong>(async (cancellationToken, progress) =>
            {
                InMemoryRandomAccessStream randomAccessStream = new InMemoryRandomAccessStream();
                DataWriter writer = new DataWriter(randomAccessStream);

                uint bytesStored = await writer.StoreAsync().AsTask(cancellationToken);

                // Make sure that the DataWriter destructor does not close the stream.
                writer.DetachStream();

                // Report progress.
                progress.Report(randomAccessStream.Size);

                return randomAccessStream.GetInputStreamAt(0);
            });
        }

        public IAsyncOperationWithProgress<string, ulong> ReadAsStringAsync()
        {
            return AsyncInfo.Run<string, ulong>((cancellationToken, progress) =>
            {
                return Task<string>.Run(() =>
                {
                    string result = "";

                    // Report progress (length of string).
                    progress.Report((ulong)result.Length);

                    return result;
                });
            });
        }

        public bool TryComputeLength(out ulong length)
        {
            length = 0;
            return false;
        }

        public IAsyncOperationWithProgress<ulong, ulong> WriteToStreamAsync(IOutputStream outputStream)
        {
            return AsyncInfo.Run<ulong, ulong>(async (cancellationToken, progress) =>
            {
                DataWriter writer = new DataWriter(outputStream);
                uint bytesWritten = await writer.StoreAsync().AsTask(cancellationToken);

                // Make sure that DataWriter destructor does not close the stream.
                writer.DetachStream();

                // Report progress.
                progress.Report(bytesWritten);

                return 0;
            });
        }

        public void Dispose()
        {
        }

        private ulong GetLength()
        {
            return 0;
        }
    }
}

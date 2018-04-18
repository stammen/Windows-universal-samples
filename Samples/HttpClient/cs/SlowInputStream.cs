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
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage.Streams;

namespace SDKTemplate
{
    class SlowInputStream : IInputStream
    {
        bool m_endOfAudio;
        uint m_audioBytesAvailable;

        public SlowInputStream(uint length)
        {
            m_endOfAudio = false;
            m_audioBytesAvailable = length;
        }
        
        // add code to add audio to buffer
        // add code to signal end of audio stream

        public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            return AsyncInfo.Run<IBuffer, uint>(async (cancellationToken, progress) =>
            {
                // Wait for enough audio or end of audio
                while ((m_audioBytesAvailable < count) && !m_endOfAudio)
                {
                    await Task.Delay(100);
                    // simulate adding audio
                    m_audioBytesAvailable = count;
                    m_endOfAudio = true;
                }

                var byteCount = m_endOfAudio ? m_audioBytesAvailable : count;
                if(byteCount > count)
                {
                    byteCount = count;
                }

                m_audioBytesAvailable -= byteCount;

                byte[] data = new byte[byteCount];
                for (uint i = 0; i < byteCount; i++)
                {
                    data[i] = 64;
                }

                progress.Report(byteCount);
                Debug.WriteLine(byteCount);
                return data.AsBuffer();
            });
        }

        public void Dispose()
        {
        }
    }
}

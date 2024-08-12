import { getNow } from '@/common/utils'

/**
 * 从缓存的帧数据中获取指定数量的帧
 * @param cachedFrames 缓存的帧数据数组
 * @param frameCount 请求获取的帧数量
 * @return []
 */
function getCachedFrames (cachedFrames, frameCount) {
  const minFrameCount = Math.min(frameCount, cachedFrames.length)
  const frames = []

  for (let i = 0; i < minFrameCount; i++) {
    const frame = cachedFrames.shift()

    if (frame) {
      frames.push(frame)
    }
  }

  return frames
}

/**
 * 合并帧，如果n+1帧里没有某个topic，会尝试复制n帧里的同名topic
 * @param frames 要合并的消息对象数组
 * @return []
 */
function mergeFrames (frames) {
  const { length } = frames
  const result = {}

  if (length < 1) {
    return result
  }

  const lastIndex = length - 1
  const lastFrame = frames[lastIndex]
  const mergedFrame = {
    ...lastFrame,
    messages: [...lastFrame.messages],
  }

  for (let i = lastIndex - 1; i >= 0; i--) {
    const currFrame = frames[i]

    currFrame.messages.forEach((currFrameMessage) => {
      const index = mergedFrame.messages
        .findIndex(mergedFrameMessage => mergedFrameMessage.topic === currFrameMessage.topic)

      if (index === -1) {
        result.merged = true
        mergedFrame.messages.push(currFrameMessage)
      }
    })
  }

  result.frame = mergedFrame

  return result
}

export default class CachedFrameManager {
  constructor () {
    this.maxFrameCount = 100
    this.lastCachedFrame = undefined // using lastCachedFrame to erase last merged frames rendering
    this.cachedFrames = []
  }

  /**
   * 如果缓冲帧多于一定值就drop掉
   * @param data
   */
  produce (data) {
    if (data) {
      this.lastCachedFrame = data
      this.cachedFrames.push(data)

      const { length } = this.cachedFrames

      if (length > this.maxFrameCount) {
        this.cachedFrames.shift()

        console.warn(`[produce] ${getNow()}: ${length} cached frames, shift 1 frame`)
      }
    }
  }

  /**
   * 获取最后一帧
   * @return {*}
   */
  consume () {
    const { length } = this.cachedFrames
    const output = `[consume] ${getNow()}: ${length} cached frames, consume ${length} frames`

    if (length > 4) {
      console.warn(output)
    }

    const cachedFrames = getCachedFrames(this.cachedFrames, length)
    const { frame, merged } = mergeFrames(cachedFrames)

    if (frame) {
      // last cached frame is not generated by merged frames, no need to render next time
      if (!merged && this.cachedFrames.length === 0) {
        this.lastCachedFrame = undefined
      }

      return frame
    }

    const { lastCachedFrame } = this

    if (lastCachedFrame) {
      this.lastCachedFrame = undefined
    }

    return lastCachedFrame
  }

  clear () {
    this.cachedFrames = []
  }
}
